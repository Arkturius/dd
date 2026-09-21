/**
 * decoder.c
 */

#include "basics.h"
#include <complex.h>
#include <decoder.h>
#include <stddef.h>
#include <utils.h>

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

	if (dp->opcode.len < sizeof(dp->opcode.raw))
		dp->opcode.raw[dp->opcode.len++] = b;
	decoder_advance(dp);
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

INTERN const OpcodeMeta
*resolve_opcode_meta(Decoder *dp, const OpcodeMeta *meta)
{
	u8	idx = 0;

	switch (meta->kind)
	{
		case META_TABLE:
			idx = decoder_skip(dp);
			break ;
		case META_SPLIT_MOD:
			decoder_ensure_modrm(dp);
			idx = dp->modrm.mod == 0b11
				? SPLIT_MOD_REG 
				: SPLIT_MOD_MEM;
			break ;
		case META_SPLIT_PFX:
			idx = SPLIT_PFX_NONE;
			if (dp->prefix.opsize)
				idx = SPLIT_PFX_66;
			if (dp->prefix.repeat == REPEAT_E)
				idx = SPLIT_PFX_F3;
			if (dp->prefix.repeat == REPEAT_NE)
				idx = SPLIT_PFX_F2;
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

INTERN OpcodeMeta
resolve_opcode(Decoder *dp)
{
	// TODO: Here should go the switch between base tables (VEX)
	//       The 32bit switch must be wider and cover the whole
	//       decode session.
	const OpcodeMeta	*meta = &opcode_meta_root;

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
		case 0xC5: return !ex.vex2.L;
		case 0xC4: return !ex.vex3.L;
		case 0x62: return !ex.evex.LL;
		default:   return 0;
	}
}

INTERN x86_Size
resolve_operand_size
(
	Decoder           *dp,
	DecodeOperandType type,
	const OpcodeMeta  *meta
)
{
	const OpcodeFlags		of = meta->flags;
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
			if (resolve_ext_W(ex))
				return SZ_YMMWORD;
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
			decode_inline_value(dp, sz);
			break ;
		case METHOD_O:
			decode_inline_value(dp, ps.addrsize ? 4 : 8);
			break ;
		case METHOD_L:
			decode_inline_value(dp, 1);
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
	for (u32 i = 0; i < dp->nops; ++i)
	{
		OpcodeOperand	op = dp->ops[i];
		x86_Size		sz = resolve_operand_size(dp, op.type, &meta);

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
decoder_materialize(Decoder *dp, x86_Instruction *ins)
{
	UNUSED(dp);
	UNUSED(ins);
}

INTERN bool
decoder_run(Decoder *dp, x86_Instructions *code)
{
	while (!decoder_empty(dp))
	{
		x86_Instruction	ins = {0};

		if (!decode_instruction(dp))
			return false;

		if (!x86_mnemonics[dp->mnemonic])
			TODO("Missing mnemonic display string - %d", dp->mnemonic);

 		INFO("< %s >", x86_mnemonics[dp->mnemonic]);
// 		INFO(" %zu / %zu bytes", dp->pc - dp->start, dp->end - dp->start);
// 		hexdump(dp->opcode.raw, dp->opcode.len);

		decoder_materialize(dp, &ins);
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

	INFO("group1 at %p", opcode_meta_group1);
	INFO("  next at %p", opcode_meta_group11_c7_111_11);
	decoder_new(&decoder, stream, len);
	if (!decoder_run(&decoder, code))
	{
//		decoder_error_report(&decoder);
		return false;
	}
	return true;
}
