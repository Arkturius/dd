/**
 * opcode_dsl.h
 */

#if !defined (_OPCODE_DSL_H)
# define _OPCODE_DSL_H

# include <basics.h>

# include <decode_types.h>
# include <x86.h>
# include <opcodes.h>

# define	CONST	static const

# define	META(_kind, _args...)	\
{									\
	.kind = META_ ## _kind,			\
	_args							\
}

# define	TABLE(_name, _args...)			META(TABLE, .split = _name, _args)
# define	SPLIT(_name, _kind, _args...)	META(SPLIT_ ## _kind, .split = _name, _args)

# define	NOPS(_ops...)		sizeof((OpcodeOperand[]){ _ops }) / sizeof(OpcodeOperand)
# define	OP(_am, _ot)		{ .method = METHOD_ ## _am, .type  = OPTYPE_ ## _ot }
# define	FX(_fx)				{ .method = METHOD_FX,      .fixed = FX_ ## _fx }

# define	D64					.default_size = DFL_SIZE_D64
# define	F64					.default_size = DFL_SIZE_F64
# define	I64					.validity     = VALIDITY_I64
# define	O64					.validity     = VALIDITY_O64
# define	Operands(_ops...)	.nops		  = NOPS(_ops), .ops = { _ops }
# define	Mnemonic(_mne)		.mnemonic     = MNEMO_ ## _mne
# define	Flags(_flags...)	.flags        = { _flags }

# define	Opcode(_byte, _args...)				[_byte] = META(OPCODE, _args)
# define	Split(_byte, _kind, _ref, _args...)	[_byte] = SPLIT(_ref, _kind, _args)

# define	Eb	OP(E, B)
# define	Ev	OP(E, V)

# define	Gb	OP(G, B)
# define	Gv	OP(G, V)

# define	Ib	OP(I, B)
# define	Iz	OP(I, Z)

# define	OPv	OP(OP, V)

# define	AL	FX(AL)
# define	RA	FX(rAX)

CONST OpcodeTable		meta_base;
CONST OpcodeTable		meta_base_0f;
CONST OpcodeTable		meta_base_0f_38;
CONST OpcodeTable		meta_base_0f_3a;

CONST OpcodeSplitReg	meta_group1;
CONST OpcodeSplitReg	meta_group2;
CONST OpcodeSplitReg	meta_group3;
CONST OpcodeSplitReg	meta_group4;
CONST OpcodeSplitReg	meta_group5;
CONST OpcodeSplitReg	meta_group6;
CONST OpcodeSplitReg	meta_group7;
CONST OpcodeSplitReg	meta_group8;
CONST OpcodeSplitReg	meta_group9;
CONST OpcodeSplitReg	meta_group10;
CONST OpcodeSplitReg	meta_group11;
CONST OpcodeSplitReg	meta_group12;
CONST OpcodeSplitReg	meta_group13;
CONST OpcodeSplitReg	meta_group14;
CONST OpcodeSplitReg	meta_group15;
CONST OpcodeSplitReg	meta_group16;

CONST OpcodeMeta	meta_root = TABLE(meta_base);
CONST OpcodeTable	meta_base = 
{
	Opcode(0x00, Mnemonic(ADD),   Operands(Eb, Gb)),
	Opcode(0x01, Mnemonic(ADD),   Operands(Ev, Gv)),
	Opcode(0x02, Mnemonic(ADD),   Operands(Gb, Eb)),
	Opcode(0x03, Mnemonic(ADD),   Operands(Gv, Ev)),
	Opcode(0x04, Mnemonic(ADD),   Operands(AL, Ib)),
	Opcode(0x05, Mnemonic(ADD),   Operands(RA, Iz)),

	Opcode(0x08, Mnemonic(OR),    Operands(Eb, Gb)),
	Opcode(0x09, Mnemonic(OR),    Operands(Ev, Gv)),
	Opcode(0x0a, Mnemonic(OR),    Operands(Gb, Eb)),
	Opcode(0x0b, Mnemonic(OR),    Operands(Gv, Ev)),
	Opcode(0x0c, Mnemonic(OR),    Operands(AL, Ib)),
	Opcode(0x0d, Mnemonic(OR),    Operands(RA, Iz)),

	Opcode(0x10, Mnemonic(ADC),   Operands(Eb, Gb)),
	Opcode(0x11, Mnemonic(ADC),   Operands(Ev, Gv)),
	Opcode(0x12, Mnemonic(ADC),   Operands(Gb, Eb)),
	Opcode(0x13, Mnemonic(ADC),   Operands(Gv, Ev)),
	Opcode(0x14, Mnemonic(ADC),   Operands(AL, Ib)),
	Opcode(0x15, Mnemonic(ADC),   Operands(RA, Iz)),

	Opcode(0x18, Mnemonic(SBB),   Operands(Eb, Gb)),
	Opcode(0x19, Mnemonic(SBB),   Operands(Ev, Gv)),
	Opcode(0x1a, Mnemonic(SBB),   Operands(Gb, Eb)),
	Opcode(0x1b, Mnemonic(SBB),   Operands(Gv, Ev)),
	Opcode(0x1c, Mnemonic(SBB),   Operands(AL, Ib)),
	Opcode(0x1d, Mnemonic(SBB),   Operands(RA, Iz)),

	Opcode(0x20, Mnemonic(AND),   Operands(Eb, Gb)),
	Opcode(0x21, Mnemonic(AND),   Operands(Ev, Gv)),
	Opcode(0x22, Mnemonic(AND),   Operands(Gb, Eb)),
	Opcode(0x23, Mnemonic(AND),   Operands(Gv, Ev)),
	Opcode(0x24, Mnemonic(AND),   Operands(AL, Ib)),
	Opcode(0x25, Mnemonic(AND),   Operands(RA, Iz)),

	Opcode(0x28, Mnemonic(SUB),   Operands(Eb, Gb)),
	Opcode(0x29, Mnemonic(SUB),   Operands(Ev, Gv)),
	Opcode(0x2a, Mnemonic(SUB),   Operands(Gb, Eb)),
	Opcode(0x2b, Mnemonic(SUB),   Operands(Gv, Ev)),
	Opcode(0x2c, Mnemonic(SUB),   Operands(AL, Ib)),
	Opcode(0x2d, Mnemonic(SUB),   Operands(RA, Iz)),
	
	Opcode(0x30, Mnemonic(XOR),   Operands(Eb, Gb)),
	Opcode(0x31, Mnemonic(XOR),   Operands(Ev, Gv)),
	Opcode(0x32, Mnemonic(XOR),   Operands(Gb, Eb)),
	Opcode(0x33, Mnemonic(XOR),   Operands(Gv, Ev)),
	Opcode(0x34, Mnemonic(XOR),   Operands(AL, Ib)),
	Opcode(0x35, Mnemonic(XOR),   Operands(RA, Iz)),

	Opcode(0x38, Mnemonic(CMP),   Operands(Eb, Gb)),
	Opcode(0x39, Mnemonic(CMP),   Operands(Ev, Gv)),
	Opcode(0x3a, Mnemonic(CMP),   Operands(Gb, Eb)),
	Opcode(0x3b, Mnemonic(CMP),   Operands(Gv, Ev)),
	Opcode(0x3c, Mnemonic(CMP),   Operands(AL, Ib)),
	Opcode(0x3d, Mnemonic(CMP),   Operands(RA, Iz)),

	Opcode(0x50, Mnemonic(PUSH),  Operands(OPv), Flags(D64)),
	Opcode(0x51, Mnemonic(PUSH),  Operands(OPv), Flags(D64)),
	Opcode(0x52, Mnemonic(PUSH),  Operands(OPv), Flags(D64)),
	Opcode(0x53, Mnemonic(PUSH),  Operands(OPv), Flags(D64)),
	Opcode(0x54, Mnemonic(PUSH),  Operands(OPv), Flags(D64)),
	Opcode(0x55, Mnemonic(PUSH),  Operands(OPv), Flags(D64)),
	Opcode(0x56, Mnemonic(PUSH),  Operands(OPv), Flags(D64)),
	Opcode(0x57, Mnemonic(PUSH),  Operands(OPv), Flags(D64)),
	Opcode(0x58, Mnemonic(POP),   Operands(OPv), Flags(D64)),
	Opcode(0x59, Mnemonic(POP),   Operands(OPv), Flags(D64)),
	Opcode(0x5a, Mnemonic(POP),   Operands(OPv), Flags(D64)),
	Opcode(0x5b, Mnemonic(POP),   Operands(OPv), Flags(D64)),
	Opcode(0x5c, Mnemonic(POP),   Operands(OPv), Flags(D64)),
	Opcode(0x5d, Mnemonic(POP),   Operands(OPv), Flags(D64)),
	Opcode(0x5e, Mnemonic(POP),   Operands(OPv), Flags(D64)),
	Opcode(0x5f, Mnemonic(POP),   Operands(OPv), Flags(D64)),

	Opcode(0x63, Mnemonic(MOVSXD), Operands(Gv, Ev), Flags(O64)),

	Opcode(0x70, Mnemonic(JO),    Operands(Eb, Ib)),
	Opcode(0x71, Mnemonic(JNO),   Operands(Eb, Ib)),
	Opcode(0x72, Mnemonic(JB),    Operands(Eb, Ib)),
	Opcode(0x73, Mnemonic(JAE),   Operands(Eb, Ib)),
	Opcode(0x74, Mnemonic(JE),    Operands(Eb, Ib)),
	Opcode(0x75, Mnemonic(JNE),   Operands(Eb, Ib)),
	Opcode(0x76, Mnemonic(JBE),   Operands(Eb, Ib)),
	Opcode(0x77, Mnemonic(JA),    Operands(Eb, Ib)),
	Opcode(0x78, Mnemonic(JS),    Operands(Eb, Ib)),
	Opcode(0x79, Mnemonic(JNS),   Operands(Eb, Ib)),
	Opcode(0x7a, Mnemonic(JP),    Operands(Eb, Ib)),
	Opcode(0x7b, Mnemonic(JNP),   Operands(Eb, Ib)),
	Opcode(0x7c, Mnemonic(JL),    Operands(Eb, Ib)),
	Opcode(0x7d, Mnemonic(JGE),   Operands(Eb, Ib)),
	Opcode(0x7e, Mnemonic(JLE),   Operands(Eb, Ib)),
	Opcode(0x7f, Mnemonic(JG),    Operands(Eb, Ib)),

	Split(0x80, REG, meta_group1, Operands(Eb, Ib)),
};

#endif
