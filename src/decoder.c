/**
 * decoder.c
 */

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

INTERN void
decode_extensions(Decoder *dp)
{
	u8	ext = decoder_get(dp);
	u8	n = 0;

	if ((ext & 0xF0) == 0x40)
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
decoder_ensure_modrm(Decoder *dp)
{
	if (dp->flags & DECODER_HAS_MODRM)
		return ;
	dp->modrm.raw = decoder_skip(dp);
	dp->flags |= DECODER_HAS_MODRM;
}

INTERN const OpcodeMeta
*decoder_resolve_opcode_meta(Decoder *dp, const OpcodeMeta *meta)
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
			return meta;
	}
	return &meta->as.split[idx];
}

INTERN OpcodeMeta
resolve_opcode(Decoder *dp)
{
	const OpcodeMeta	*meta = &opcode_meta_root;

	while (true)
	{
		for (u32 i = 0; i < meta->nops; ++i)
			dp->ops[dp->nops++] = meta->ops[i];
		if (meta->kind == META_OPCODE || meta->kind == META_INVALID)
			break ;
		meta = decoder_resolve_opcode_meta(dp, meta);
	}
	return *meta;
}

INTERN x86_Size
resolve_operand_size(const OpcodeMeta *meta, DecodeOperandType t)
{
	if (meta->as.opcode.flags.default_size == DFL_SIZE_F64)
		return SZ_QWORD;
	switch (t)
	{
		case OPTYPE_B:
			return SZ_BYTE;
		default:
			break ;
	}
	TODO("Handle DecodeOperandType(%d)", t);
	return SZ_DWORD;
}

typedef void	(*DecodeMethod)(Decoder *, x86_Size);

INTERN void
decode_method_none(Decoder *dp, x86_Size size)
{
	UNUSED(dp);
	UNUSED(size);
	INFO("This method is a NO-OP");
}

static const DecodeMethod
addressing_method_decoders[enum_count(DecodeAddressingMethod)] =
{
	[0x00 ... enum_count(DecodeAddressingMethod) - 1] = decode_method_none,
};

INTERN bool
decode_opcode(Decoder *dp)
{
	const OpcodeMeta	meta = resolve_opcode(dp);

	if (meta.kind == META_INVALID)
		TODO("Handle Byte(%02xh)", decoder_opcode_last(dp));

	for (u32 i = 0; i < dp->nops; ++i)
	{
		OpcodeOperand	op = dp->ops[i];
		DecodeMethod	fn = addressing_method_decoders[op.method];
		x86_Size		sz = resolve_operand_size(&meta, op.type);

		if (fn == decode_method_none)
			TODO("DecodeAddressingMethod(%d)", op.method);
		fn(dp, sz);
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

		hexdump(dp->opcode.raw, dp->opcode.len);

//		decoder_skip(dp);
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

	decoder_new(&decoder, stream, len);
	if (!decoder_run(&decoder, code))
	{
//		decoder_error_report(&decoder);
		return false;
	}
	return true;
}
