/**
 * decoder.h
 */

#if !defined (_DECODER_H)
# define _DECODER_H

# include <basics.h>
# include <opcodes.h>
# include <decode_types.h>

# define DECODER_HAS_MODRM	BIT(0)
# define DECODER_HAS_REG_OP	BIT(1)

typedef struct Decoder
{
	u8	*start;
	u8	*pc;
	u8	*end;
	u8	reset[0];

	DecodeOpcode		opcode;
	DecodePrefixState	prefix;
	DecodeExtension		extension;
	DecodeModRM			modrm;
	DecodeSib			sib;
	DecodeDisp			disp;
	DecodeImmediate		imm;

	u8				flags;
	u8				nops;
	OpcodeOperand	ops[4];
	OpcodeFlags		opflags;

	x86_Size		addr_size;
	x86_Mnemonic	mnemonic;
}
Decoder;

array_type(x86_Instruction, x86_Instructions);

bool
decode(x86_Instructions *code, u8 *stream, u32 len);

#endif // _DECODER_H
