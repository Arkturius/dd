/**
 * decoder.c
 */

#include "basics.h"
#include <complex.h>
#include <decoder.h>
#include <stddef.h>
#include <utils.h>

# include <opcode_dsl.h>

INTERN void
decoder_new(Decoder *dp, u8 *stream, u32 len)
{
	dp->start = stream;
	dp->pc    = stream;
	dp->end   = stream + len;
}

INTERN bool
decoder_empty(Decoder *dp)
{
	return dp->pc >= dp->end;
}

INTERN void
decoder_clear(Decoder *dp)
{
	u8			*opcode_ptr = (u8 *)&dp->opcode;
	const u32	size        = sizeof(*dp) - offsetof(Decoder, reset);

	memset(opcode_ptr, 0, size);
}

INTERN u8
decoder_get(Decoder *dp)
{
	return *dp->pc;
}

INTERN u8
decoder_opcode_last(Decoder *dp)
{
	return dp->opcode.raw[dp->opcode.len - !!dp->opcode.len];
}

INTERN void
decoder_advance(Decoder *dp)
{
	dp->pc++;
}

INTERN u8
decoder_skip(Decoder *dp)
{
	if (decoder_empty(dp))
		return 0;

	u8	b = decoder_get(dp);

	decoder_advance(dp);
	return b;
}

INTERN u8
decoder_opcode_skip(Decoder *dp)
{
	u8	b = decoder_skip(dp);

	if (b && dp->opcode.len < sizeof(dp->opcode.raw))
		dp->opcode.raw[dp->opcode.len++] = b;
	return b;
}

INTERN u64
decode_inline_value(Decoder *dp, u8 n)
{
	union { u8 b; u16 w; u32 d; u64 q; } v = {0};

	assert(!n || n == 1 || n == 2 || n == 4 || n == 8);
	for (u32 i = 0; i < n; ++i)
		((u8 *)&v)[i] = decoder_skip(dp);
	switch (n)
	{
		case 0: return 0;
		case 1: return v.b;
		case 2: return v.w;
		case 4: return v.d;
		case 8: return v.q;
	}
	UNREACHABLE();
}

INTERN void
decode_legacy_prefixes(Decoder *dp)
{
	do
	{
		u8	pfx = decoder_get(dp);

		switch (pfx)
		{
			case 0xF0: dp->prefix.lock     = true;       break ;
			case 0xF2: dp->prefix.repeat   = REPEAT_NE;  break ;
			case 0xF3: dp->prefix.repeat   = REPEAT_E;   break ;
			case 0x26: dp->prefix.segment  = SEGMENT_ES; break ;
			case 0x2E: dp->prefix.segment  = SEGMENT_CS; break ;
			case 0x36: dp->prefix.segment  = SEGMENT_SS; break ;
			case 0x3E: dp->prefix.segment  = SEGMENT_DS; break ;
			case 0x64: dp->prefix.segment  = SEGMENT_FS; break ;
			case 0x65: dp->prefix.segment  = SEGMENT_GS; break ;
			case 0x66: dp->prefix.opsize   = true;       break ;
			case 0x67: dp->prefix.addrsize = true;       break ;
			default:   return ;
		}
		decoder_skip(dp);
	} while (1);
}

# define	IS_REX(_b)	(((_b) & 0xF0) == 0x40)

INTERN void
decode_extensions(Decoder *dp)
{
	u8	ext = decoder_get(dp);
	u8	n = 0;

	if IS_REX(ext)
		n = 1;
	else switch (ext)
	{
		case 0xC5: n = 2; break ;
		case 0xC4: n = 3; break ;
		case 0x62: n = 4; break ;
		default:  return ;
	}
	for (u32 i = 0; i < n; ++i)
		dp->extension.raw[i] = decoder_skip(dp);
	return ;
}

INTERN void
decode_disp(Decoder *dp)
{
	if (dp->disp.size)
		dp->disp.disp = decode_inline_value(dp, dp->disp.size);
}

INTERN void
decode_sib(Decoder *dp)
{
	DecodeModRM	modrm = dp->modrm;
	DecodeSib	sib;

	if (modrm.mod != 0b11 && modrm.rm == 0b100)
	{
		sib.raw = decoder_skip(dp);
		if (sib.base == 0b101 && modrm.mod == 0b00)
			dp->disp.size = 4;
		dp->sib = sib;
	}
}

INTERN void
decode_modrm(Decoder *dp)
{
	DecodeModRM	modrm;

	modrm.raw = decoder_skip(dp);
	switch (modrm.mod)
	{
		case 0b01: dp->disp.size = 1; break ;
		case 0b10: dp->disp.size = 4; break ;
		default:                      break ;
	}
	if (modrm.mod == 0b00 && modrm.rm == 0b101)
		dp->disp.size = 4;
	dp->modrm = modrm;
}

INTERN void
decoder_ensure_modrm(Decoder *dp)
{
	if (dp->flags & DECODER_HAS_MODRM)
		return ;
	dp->flags |= DECODER_HAS_MODRM;
	decode_modrm(dp);
	decode_sib(dp);
	decode_disp(dp);
}

INTERN u8
resolve_ext_p(const DecodeExtension ex)
{
	switch (ex.raw[0])
	{
		case 0xC5: return ex.vex2.p;
		case 0xC4: return ex.vex3.p;
		case 0x62: return ex.evex.p;
		default:
			UNREACHABLE();
	}
}

INTERN u8
resolve_ext_m(const DecodeExtension ex)
{
	switch (ex.raw[0])
	{
		case 0xC5: return 0b00001;
		case 0xC4: return ex.vex3.m;
		case 0x62: return ex.evex.m;
		default:
			UNREACHABLE();
	}
}

INTERN u8
decode_prefix_split(Decoder *dp)
{
	u8	idx;

	if (dp->extension.raw[0] == 0xC4 || dp->extension.raw[0] == 0xC5)
		idx = resolve_ext_p(dp->extension);
	else
	{
		idx = SPLIT_PFX_NONE;
		if (dp->prefix.opsize)
			idx = SPLIT_PFX_66;
		if (dp->prefix.repeat == REPEAT_E)
			idx = SPLIT_PFX_F3;
		if (dp->prefix.repeat == REPEAT_NE)
			idx = SPLIT_PFX_F2;
	}
	return idx;
}

INTERN const OpcodeMeta
*resolve_opcode_meta(Decoder *dp, const OpcodeMeta *meta)
{
	u8	idx = 0;

	switch (meta->kind)
	{
		case META_TABLE:
			idx = decoder_opcode_skip(dp);
			break ;
		case META_SPLIT_MOD:
			decoder_ensure_modrm(dp);
			idx = dp->modrm.mod == 0b11
				? SPLIT_MOD_REG 
				: SPLIT_MOD_MEM;
			break ;
		case META_SPLIT_PFX:
			idx = decode_prefix_split(dp);
			break ;
		case META_SPLIT_REG:
			decoder_ensure_modrm(dp);
			idx = dp->modrm.reg;
			break ;
		case META_SPLIT_RM:
			decoder_ensure_modrm(dp);
			idx = dp->modrm.rm;
			break ;
		default:
			UNREACHABLE();
	}
	return &meta->split[idx];
}

INTERN const OpcodeMeta
*resolve_meta_root(Decoder *dp)
{
	const OpcodeMeta	*root = &meta_root;

	if 
	(
		dp->extension.raw[0] == 0xC4 || 
		dp->extension.raw[0] == 0xC5 || 
		dp->extension.raw[0] == 0x62
	)
	{
		switch (resolve_ext_m(dp->extension))
		{
			case 0b00001: root = &meta_root_vex;    break ;
			case 0b00010: root = &meta_root_vex_38; break ;
			case 0b00011: root = &meta_root_vex_3a; break ;
			default:
				UNREACHABLE(); // TODO: 20260923-145816
		}
	}
	return root;
}

INTERN OpcodeMeta
resolve_opcode(Decoder *dp)
{
	// TODO: Here should go the switch between base tables (VEX)
	//       The 32bit switch must be wider and cover the whole
	//       decode session.

	const OpcodeMeta	*meta = resolve_meta_root(dp);
//	const OpcodeMeta	*meta = &meta_root;

	while (true)
	{
		for (u32 i = 0; i < meta->nops; ++i)
			dp->ops[dp->nops++] = meta->ops[i];
		if (meta->mnemonic != MNEMO_INVALID)
			dp->mnemonic = meta->mnemonic;
		if (meta->kind == META_OPCODE || meta->kind == META_INVALID)
			break ;
		meta = resolve_opcode_meta(dp, meta);
	}
	return *meta;
}

INTERN u8
resolve_ext_W(const DecodeExtension ex)
{
	switch (ex.raw[0])
	{
		case 0xC4: return !ex.vex3.W;
		case 0x62: return !ex.evex.W;
		default:   return IS_REX(ex.raw[0]) ? ex.rex.W : 0;
	}
}

INTERN u8
resolve_ext_R(const DecodeExtension ex)
{
	switch (ex.raw[0])
	{
		case 0xC5: return !ex.vex2.R;
		case 0xC4: return !ex.vex3.R;
		case 0x62: return !ex.evex.R;
		default:   return IS_REX(ex.raw[0]) ? ex.rex.R : 0;
	}
}

INTERN u8
resolve_ext_X(const DecodeExtension ex)
{
	switch (ex.raw[0])
	{
		case 0xC4: return !ex.vex3.X;
		case 0x62: return !ex.evex.X;
		default:   return IS_REX(ex.raw[0]) ? ex.rex.X : 0;
	}
}

INTERN u8
resolve_ext_B(const DecodeExtension ex)
{
	switch (ex.raw[0])
	{
		case 0xC4: return !ex.vex3.B;
		case 0x62: return !ex.evex.B;
		default:   return IS_REX(ex.raw[0]) ? ex.rex.B : 0;
	}
}

INTERN u8
resolve_ext_L(const DecodeExtension ex)
{
	switch (ex.raw[0])
	{
		case 0xC5: return ex.vex2.L;
		case 0xC4: return ex.vex3.L;
		case 0x62: return ex.evex.LL;
		default:   return 0;
	}
}

INTERN x86_Size
resolve_operand_size (Decoder *dp, DecodeOperandType type)
{
	const OpcodeFlags		of = dp->opflags;
	const DecodeExtension	ex = dp->extension;
	const DecodePrefixState	ps = dp->prefix;

	switch (type)
	{
		case OPTYPE_0:
			FALLTHROUGH;
		case OPTYPE_B:
			return SZ_BYTE;
		case OPTYPE_W:
			return SZ_WORD;
		case OPTYPE_Q:
			return SZ_QWORD;
		case OPTYPE_V:
			if (resolve_ext_W(ex))
				return SZ_QWORD;
			if (of.default_size == DFL_SIZE_F64)
				return SZ_QWORD;
			if (ps.opsize)
				return SZ_WORD;
			if (of.default_size == DFL_SIZE_D64)
				return SZ_QWORD;
			return SZ_DWORD;
		case OPTYPE_Y:
			if (resolve_ext_W(ex))
				return SZ_QWORD;
			return SZ_DWORD;
		case OPTYPE_Z:
			if (resolve_ext_W(ex))
				return SZ_DWORD;
			if (ps.opsize)
				return SZ_WORD;
			return SZ_DWORD;
		case OPTYPE_SS:
			FALLTHROUGH;
		case OPTYPE_SD:
			return SZ_XMMWORD;
		case OPTYPE_PS:
			FALLTHROUGH;
		case OPTYPE_PD:
			assert(resolve_ext_L(ex) < 3);
			return SZ_XMMWORD + resolve_ext_L(ex);
		case OPTYPE_X:
			assert(resolve_ext_L(ex) < 2);
			return SZ_XMMWORD + resolve_ext_L(ex);
			FALLTHROUGH;
		case OPTYPE_DQ:
			return SZ_XMMWORD;
		case OPTYPE_QQ:
			return SZ_YMMWORD;
		default:
			break ;
	}
	TODO("Handle DecodeOperandType(%d)", type);
	return SZ_DWORD;
}

INTERN void
decode_immediate(Decoder *dp, u8 n)
{
	dp->imm.size = n;
	dp->imm.imm  = decode_inline_value(dp, n);
}

INTERN void
resolve_operand_method
(
	Decoder                *dp,
	DecodeAddressingMethod method, 
	const x86_Size         sz
)
{
	const DecodePrefixState	ps = dp->prefix;

	switch (method)
	{
		case METHOD_0:  FALLTHROUGH;
		case METHOD_A:  UNREACHABLE(); // TODO: #UD
		case METHOD_I:  FALLTHROUGH;
		case METHOD_J:
			decode_immediate(dp, sz);
			break ;
		case METHOD_O:
			decode_immediate(dp, ps.addrsize ? 4 : 8);
			break ;
		case METHOD_L:
			decode_immediate(dp, 1);
			break ;
		default:
			decoder_ensure_modrm(dp);
		case METHOD_F:  FALLTHROUGH;
		case METHOD_X:  FALLTHROUGH;
		case METHOD_Y:  FALLTHROUGH;
		case METHOD_OP: FALLTHROUGH;
		case METHOD_FX: break ;
	}
}

INTERN bool
decode_opcode(Decoder *dp)
{
	const OpcodeMeta	meta = resolve_opcode(dp);

	if (meta.kind == META_INVALID)
	{
		for (u32 i = 0; i < dp->opcode.len; ++i)
			INFO("Opcode byte: %02x", dp->opcode.raw[i]);
		TODO("Handle Byte(%02xh)", decoder_opcode_last(dp));
	}
	dp->opflags = meta.flags;
	for (u32 i = 0; i < dp->nops; ++i)
	{
		OpcodeOperand	op = dp->ops[i];
		x86_Size		sz = 0;

		if (op.method != METHOD_FX)
			sz = resolve_operand_size(dp, op.type);
		resolve_operand_method(dp, op.method, 1 << sz);
	}
	return true;
}

INTERN bool
decode_instruction(Decoder *dp)
{
	decode_legacy_prefixes(dp);
	decode_extensions(dp);

	return decode_opcode(dp);
}

INTERN void
materialize_mnemonic(Decoder *dp, x86_Instruction *ins)
{
	ins->mnemo = dp->mnemonic;
}

INTERN void
materialize_operand_fixed(Decoder *dp, x86_Operand *op, DecodeOperandFixed fx)
{
	TODO("");
	UNUSED(dp);
	UNUSED(op);

	switch (fx)
	{
		case FX_1:
		{
			op->kind   = OPKIND_IMMEDIATE;
			op->as.imm = 1;
			op->size   = SZ_BYTE;
		} break ;
		case FX_3:
		case FX_AL:
		case FX_AH:
		case FX_AX:
		case FX_CL:
		case FX_DX:
		case FX_CS:
		case FX_DS:
		case FX_FS:
		case FX_GS:
		case FX_SS:
		case FX_rAX:
		case FX_rDX:
		case FX_rCX:
		case FX_ST0:
		case FX_ST1:
		default:
			TODO("");
	}
}

INTERN void
materialize_memory(Decoder *dp, x86_Operand *op)
{
	op->kind = OPKIND_MEMORY;
	op->as.mem = (x86_Memory)
	{
		.base.size  = SZ_QWORD,
		.index.size = SZ_QWORD,
		.scale      = SCALE_BYTE,
		.disp       = dp->disp.size ? dp->disp.disp : 0,
	};
	if (dp->modrm.mod != 0b11 && dp->modrm.rm == 0b100)
	{
		op->as.mem.base.id  = dp->sib.base | (resolve_ext_B(dp->extension) << 3);
		op->as.mem.base.c   = REG_CLASS_GPR;

		op->as.mem.index.id = dp->sib.index;
		op->as.mem.index.c  = REG_CLASS_GPR;

		op->as.mem.scale    = dp->sib.scale;

		if (dp->sib.base == 0b101 && dp->modrm.mod == 0b00)
			op->as.mem.base = (x86_Register){0};
		if (dp->sib.index == 0b100)
			op->as.mem.index = (x86_Register){0};
	}
	else if (dp->modrm.mod == 0b00 && dp->modrm.rm == 0b101)
	{
		op->as.mem.base.id = REG_ID_0;
		op->as.mem.base.c  = REG_CLASS_RIP;
		op->as.mem.index = (x86_Register){0};
	}
	else
	{
		op->as.mem.base.id = dp->modrm.rm | (resolve_ext_B(dp->extension) << 3);
		op->as.mem.base.c  = REG_CLASS_GPR;
		op->as.mem.index = (x86_Register){0};
	}
}

INTERN void
materialize_gpr(Decoder *dp, x86_Operand *op, x86_RegisterId id, u8 ext_bit)
{
	op->kind        = OPKIND_REGISTER;
	op->as.reg.c    = REG_CLASS_GPR;
	op->as.reg.id   = id | (ext_bit << 3);
	op->as.reg.size = op->size;
	op->as.reg.hi8  = (op->size == SZ_BYTE)
					  && (id >= 4 && id <= 7)
	                  && (dp->extension.raw[0] == 0);
}

INTERN void
materialize_operand_typed(Decoder *dp, x86_Operand *op, DecodeAddressingMethod method)
{
	switch (method)
	{
		case METHOD_OP:
		{
			x86_RegisterId	id = (decoder_opcode_last(dp) & 0b111);
			u8				ex = resolve_ext_B(dp->extension);

			materialize_gpr(dp, op, id, ex);
		} break ;

		case METHOD_E:
		{
			if (dp->modrm.mod == 0b11)
			{
				x86_RegisterId	id = dp->modrm.rm;
				u8				ex = resolve_ext_B(dp->extension);

				materialize_gpr(dp, op, id, ex);
			}
			else
				materialize_memory(dp, op);
		} break ;
		case METHOD_G:
		{
			x86_RegisterId	id = dp->modrm.reg;
			u8				ex = resolve_ext_R(dp->extension);

			materialize_gpr(dp, op, id, ex);
		} break ;
		case METHOD_M:
		{
			materialize_memory(dp, op);
		} break ;
		case METHOD_I: 
			FALLTHROUGH;
		case METHOD_J:
			FALLTHROUGH;
		case METHOD_O:
		{
			op->kind   = OPKIND_IMMEDIATE;
			op->as.imm = dp->imm.imm; // TODO: 20260925-132609
		} break ;
		default:
			TODO("operand method [%d]", method);
	}
}

INTERN void
materialize_operand(Decoder *dp, x86_Operand *op, OpcodeOperand raw)
{
	op->size = resolve_operand_size(dp, raw.type);
	if (raw.method == METHOD_FX)
		materialize_operand_fixed(dp, op, raw.fixed);
	else
		materialize_operand_typed(dp, op, raw.method);
}

INTERN void
decoder_materialize(Decoder *dp, x86_Instruction *ins)
{
	materialize_mnemonic(dp, ins);

	for (u32 i = 0; i < dp->nops; ++i)
	{
		OpcodeOperand	raw_op = dp->ops[i];

		materialize_operand(dp, &ins->ops[i], raw_op);
		if (ins->ops[i].kind == OPKIND_REGISTER)
			dp->flags |= DECODER_HAS_REG_OP;
	}
}

INTERN void
print_immediate(Buffer *out, x86_Immediate imm, x86_Size size)
{
	UNUSED(size);
	buf_appendf(out, "0x%lx", (i64)imm);
}

INTERN void
print_register_gpr(Buffer *out, x86_Register reg, x86_Size size)
{
	if (reg.hi8)
	{
		assert(reg.id >= 4 && reg.id <= 7);
		buf_appendf(out, "%s", reg_gpr_names_hi8[reg.id - 4]);
		return ;
	}
	assert(reg.size <= SZ_QWORD);
	buf_appendf(out, "%s", reg_gpr_names[reg.id][size]);
}

INTERN void
print_register_seg(Buffer *out, x86_Register reg)
{
	buf_appendf(out, "%s", reg_seg_names[reg.id]);
}

INTERN void
print_register(Buffer *out, x86_Register reg, x86_Size size)
{
	switch (reg.c)
	{
		case REG_CLASS_GPR: print_register_gpr(out, reg, size); break ;
		case REG_CLASS_SEG: print_register_seg(out, reg);       break ;
		case REG_CLASS_RIP: buf_append(out, "rip");			    break ;
		case REG_CLASS_FLG:
		case REG_CLASS_X87:
		case REG_CLASS_MMX:
		case REG_CLASS_VEC:
		case REG_CLASS_MSK:
		case REG_CLASS_BND:
		case REG_CLASS_CTL:
		case REG_CLASS_DBG:
		case REG_CLASS_SYS:
		default:
			TODO("print register class %d", reg.c);
	}
}

INTERN void
print_memory(Buffer *out, x86_Memory mem, x86_Size size)
{
	buf_appendf(out, "%s PTR ", reg_size_prefixes[size]);
	buf_appendf(out, "[");

	if (mem.base.size)
	{
		print_register(out, mem.base, SZ_QWORD);
		if (mem.index.size)
			buf_appendf(out, " + ");
	}
	if (mem.index.size)
	{
		print_register(out, mem.index, SZ_QWORD);
		if (mem.scale > SCALE_BYTE)
			buf_appendf(out, " * %d", 1 << mem.scale);
	}
	if (mem.disp > 0)
	{
		buf_append(out, " + ");
		print_immediate(out, mem.disp, 0);
	}
	if (mem.disp < 0)
	{
		buf_append(out, " - ");
		print_immediate(out, -mem.disp, 0);
	}
	buf_appendf(out, "]");
}

INTERN void
print_operand(x86_Operand op)
{
	buf_with(out)
	{
		switch (op.kind)
		{
			case OPKIND_IMMEDIATE:
				print_immediate(&out, op.as.imm, op.size);
				break ;
			case OPKIND_REGISTER:
				print_register(&out, op.as.reg, op.size);
				break ;
			case OPKIND_MEMORY:
				print_memory(&out, op.as.mem, op.size);
				break ;
			default:
				UNREACHABLE();
		}
		buf_print(&out);
	}	
}

INTERN bool
decoder_run(Decoder *dp, x86_Instructions *code)
{
	while (!decoder_empty(dp))
	{
		x86_Instruction	ins = {0};

		u32	addr = dp->pc - dp->start;

		if (!decode_instruction(dp))
			return false;

		if (!x86_mnemonics[dp->mnemonic])
			TODO("Missing mnemonic display string - %d", dp->mnemonic);

		buf_with(out)
		{
			u32 i = 0;

			buf_appendf(&out, "%8x:       ", addr);
			for (; i < dp->opcode.len && i < 15; ++i)
				buf_appendf(&out, "%02x ", dp->opcode.raw[i]);
			buf_appendf(&out, "%*.s    %s    ", 3 * (15 - i), " ", x86_mnemonics[dp->mnemonic]);
			buf_print(&out);
		}

		if (dp->mnemonic == MNEMO_RET)
			printf("\n");

//  		INFO("< %s >", x86_mnemonics[dp->mnemonic]);
// 		INFO(" %zu / %zu bytes", dp->pc - dp->start, dp->end - dp->start);
// 		hexdump(dp->opcode.raw, dp->opcode.len);

//		BREAKPOINT;
		decoder_materialize(dp, &ins);

		for(u32 i = 0; i < 4 && ins.ops[i].kind != OPKIND_NONE; ++i)
		{
			x86_Operand	op = ins.ops[i];

			if (i)
				printf(", ");
			print_operand(op);
		}
		printf("\n");
		decoder_clear(dp);

		array_push(code, ins);
	}
	return true;
}

bool
decode(x86_Instructions *code, u8 *stream, u32 len)
{
	if (!stream || !code || len == 0)
		return false;

	Decoder	decoder = {0};

	decoder_new(&decoder, stream, len);
	if (!decoder_run(&decoder, code))
	{
//		decoder_error_report(&decoder);
		return false;
	}
	return true;
}
