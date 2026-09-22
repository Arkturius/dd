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
# define	META_TABLE(_name, _args...)	\
	META(TABLE, .split = _name, _args)

# define	META_SPLIT(_kind, _name, _args...)	\
	META(SPLIT_ ## _kind, .split = _name, _args)

# define	TABLE(_name, _args...)			
# define	SPLIT(_name, _kind, _args...)	

# define	NOPS(_ops...)		sizeof((OpcodeOperand[]){ _ops }) / sizeof(OpcodeOperand)
# define	OP(_am, _ot)		{ .method = METHOD_ ## _am, .type  = OPTYPE_ ## _ot }
# define	FX(_fx)				{ .method = METHOD_FX,      .fixed = FX_ ## _fx }

# define	D64				.default_size = DFL_SIZE_D64
# define	F64				.default_size = DFL_SIZE_F64
# define	I64				.validity     = VALIDITY_I64
# define	O64				.validity     = VALIDITY_O64
# define	PRMA			.promotable   = PROMO_ADDR
# define	PRMO			.promotable   = PROMO_OP
# define	LCK				.lockable     = true
# define	PRV				.privileged   = true

# define	O(_ops...)		.nops		  = NOPS(_ops), .ops = { _ops }
# define	M(_mne)			.mnemonic     = MNEMO_ ## _mne
# define	F(_flags...)	.flags        = { _flags }

# define	Opcode(_byte, _args...)\
	[_byte] = META(OPCODE, _args)

# define	Split(_byte, _kind, _name, _args...)\
	[_byte] = META_SPLIT(_kind, _name, _args)

# define	Table(_byte, _name, _args...)\
	[_byte] = META_TABLE(_name, _args)

# define	Ap	OP(A,  P)

# define	Eb	OP(E,  B)
# define	Gb	OP(G,  B)
# define	Ib	OP(I,  B)
# define	Jb	OP(J,  B)
# define	Xb	OP(X,  B)
# define	Yb	OP(Y,  B)
# define	OPb	OP(OP, B)

# define	Pq	OP(P,  Q)
# define	Qq	OP(Q,  Q)

# define	Ev	OP(E,  V)
# define	Gv	OP(G,  V)
# define	Iv	OP(I,  V)
# define	Mv	OP(M,  V)
# define	OPv	OP(OP, V)

# define	Ew	OP(E,  W)
# define	Iw	OP(I,  W)

# define	Vx	OP(V,  X)
# define	Wx	OP(W,  X)

# define	Iz	OP(I,  Z)
# define	Jz	OP(J,  Z)

# define	Vps	OP(V,  PS)
# define	Wps	OP(W,  PS)

# define	Vpd	OP(V,  PD)
# define	Wpd	OP(W,  PD)

# define	Vss	OP(V,  PD)
# define	Wss	OP(W,  PD)

# define	Vsd	OP(V,  PD)
# define	Wsd	OP(W,  PD)

# define	ONE	FX(1)
# define	THR	FX(3)

# define	AL	FX(AL)
# define	AH	FX(AH)
# define	AX	FX(AX)

# define	CL	FX(CL)
# define	DX	FX(DX)

# define	CS	FX(CS)
# define	DS	FX(DS)
# define	FS	FX(FS)
# define	GS	FX(GS)
# define	SS	FX(SS)

# define	RA	FX(rAX)
# define	RC	FX(rCX)
# define	RD	FX(rDX)

CONST OpcodeTable		meta_base;
CONST OpcodeTable		meta_base_0f;
CONST OpcodeTable		meta_base_0f_38;
CONST OpcodeTable		meta_base_0f_3a;

CONST OpcodeSplitReg	meta_group1;
// CONST OpcodeSplitReg	meta_group1a;
CONST OpcodeSplitReg	meta_group2;
// CONST OpcodeSplitReg	meta_group3_f6;
// CONST OpcodeSplitReg	meta_group3_f7;
// CONST OpcodeSplitReg	meta_group4;
// CONST OpcodeSplitReg	meta_group5;
// CONST OpcodeSplitReg	meta_group6;
// CONST OpcodeSplitReg	meta_group7;
// CONST OpcodeSplitReg	meta_group8;
// CONST OpcodeSplitReg	meta_group9;
// CONST OpcodeSplitReg	meta_group10;
// CONST OpcodeSplitReg	meta_group11_c6;
// CONST OpcodeSplitReg	meta_group11_c7;
// CONST OpcodeSplitReg	meta_group12;
// CONST OpcodeSplitReg	meta_group13;
// CONST OpcodeSplitReg	meta_group14;
// CONST OpcodeSplitReg	meta_group15;
// CONST OpcodeSplitReg	meta_group16;

CONST OpcodeMeta
meta_root = META_TABLE(meta_base);

CONST OpcodeTable	
meta_base = 
{
	Opcode(0x00, M(ADD), O(Eb, Gb)),
	Opcode(0x01, M(ADD), O(Ev, Gv)),
	Opcode(0x02, M(ADD), O(Gb, Eb)),
	Opcode(0x03, M(ADD), O(Gv, Ev)),
	Opcode(0x04, M(ADD), O(AL, Ib)),
	Opcode(0x05, M(ADD), O(RA, Iz)),

	Opcode(0x08, M(OR), O(Eb, Gb)),
	Opcode(0x09, M(OR), O(Ev, Gv)),
	Opcode(0x0a, M(OR), O(Gb, Eb)),
	Opcode(0x0b, M(OR), O(Gv, Ev)),
	Opcode(0x0c, M(OR), O(AL, Ib)),
	Opcode(0x0d, M(OR), O(RA, Iz)),
	
	Opcode(0x0e, M(PUSH), O(CS), F(I64)),

	Table(0x0f, meta_base_0f),

	Opcode(0x10, M(ADC), O(Eb, Gb)),
	Opcode(0x11, M(ADC), O(Ev, Gv)),
	Opcode(0x12, M(ADC), O(Gb, Eb)),
	Opcode(0x13, M(ADC), O(Gv, Ev)),
	Opcode(0x14, M(ADC), O(AL, Ib)),
	Opcode(0x15, M(ADC), O(RA, Iz)),

	Opcode(0x18, M(SBB), O(Eb, Gb)),
	Opcode(0x19, M(SBB), O(Ev, Gv)),
	Opcode(0x1a, M(SBB), O(Gb, Eb)),
	Opcode(0x1b, M(SBB), O(Gv, Ev)),
	Opcode(0x1c, M(SBB), O(AL, Ib)),
	Opcode(0x1d, M(SBB), O(RA, Iz)),

	Opcode(0x20, M(AND), O(Eb, Gb)),
	Opcode(0x21, M(AND), O(Ev, Gv)),
	Opcode(0x22, M(AND), O(Gb, Eb)),
	Opcode(0x23, M(AND), O(Gv, Ev)),
	Opcode(0x24, M(AND), O(AL, Ib)),
	Opcode(0x25, M(AND), O(RA, Iz)),

	Opcode(0x28, M(SUB), O(Eb, Gb)),
	Opcode(0x29, M(SUB), O(Ev, Gv)),
	Opcode(0x2a, M(SUB), O(Gb, Eb)),
	Opcode(0x2b, M(SUB), O(Gv, Ev)),
	Opcode(0x2c, M(SUB), O(AL, Ib)),
	Opcode(0x2d, M(SUB), O(RA, Iz)),
	
	Opcode(0x30, M(XOR), O(Eb, Gb)),
	Opcode(0x31, M(XOR), O(Ev, Gv)),
	Opcode(0x32, M(XOR), O(Gb, Eb)),
	Opcode(0x33, M(XOR), O(Gv, Ev)),
	Opcode(0x34, M(XOR), O(AL, Ib)),
	Opcode(0x35, M(XOR), O(RA, Iz)),

	Opcode(0x38, M(CMP), O(Eb, Gb)),
	Opcode(0x39, M(CMP), O(Ev, Gv)),
	Opcode(0x3a, M(CMP), O(Gb, Eb)),
	Opcode(0x3b, M(CMP), O(Gv, Ev)),
	Opcode(0x3c, M(CMP), O(AL, Ib)),
	Opcode(0x3d, M(CMP), O(RA, Iz)),

	Opcode(0x50, M(PUSH), O(OPv), F(D64)),
	Opcode(0x51, M(PUSH), O(OPv), F(D64)),
	Opcode(0x52, M(PUSH), O(OPv), F(D64)),
	Opcode(0x53, M(PUSH), O(OPv), F(D64)),
	Opcode(0x54, M(PUSH), O(OPv), F(D64)),
	Opcode(0x55, M(PUSH), O(OPv), F(D64)),
	Opcode(0x56, M(PUSH), O(OPv), F(D64)),
	Opcode(0x57, M(PUSH), O(OPv), F(D64)),
	Opcode(0x58, M(POP), O(OPv), F(D64)),
	Opcode(0x59, M(POP), O(OPv), F(D64)),
	Opcode(0x5a, M(POP), O(OPv), F(D64)),
	Opcode(0x5b, M(POP), O(OPv), F(D64)),
	Opcode(0x5c, M(POP), O(OPv), F(D64)),
	Opcode(0x5d, M(POP), O(OPv), F(D64)),
	Opcode(0x5e, M(POP), O(OPv), F(D64)),
	Opcode(0x5f, M(POP), O(OPv), F(D64)),

	Opcode(0x63, M(MOVSXD), O(Gv, Ev), F(O64)),
	Opcode(0x68, M(PUSH), O(Iz), F(D64)),

	Opcode(0x70, M(JO),  O(Eb, Ib)),
	Opcode(0x71, M(JNO), O(Eb, Ib)),
	Opcode(0x72, M(JB),  O(Eb, Ib)),
	Opcode(0x73, M(JAE), O(Eb, Ib)),
	Opcode(0x74, M(JE),  O(Eb, Ib)),
	Opcode(0x75, M(JNE), O(Eb, Ib)),
	Opcode(0x76, M(JBE), O(Eb, Ib)),
	Opcode(0x77, M(JA),  O(Eb, Ib)),
	Opcode(0x78, M(JS),  O(Eb, Ib)),
	Opcode(0x79, M(JNS), O(Eb, Ib)),
	Opcode(0x7a, M(JP),  O(Eb, Ib)),
	Opcode(0x7b, M(JNP), O(Eb, Ib)),
	Opcode(0x7c, M(JL),  O(Eb, Ib)),
	Opcode(0x7d, M(JGE), O(Eb, Ib)),
	Opcode(0x7e, M(JLE), O(Eb, Ib)),
	Opcode(0x7f, M(JG),  O(Eb, Ib)),

	Split(0x80, REG, meta_group1, O(Eb, Ib)),
	Split(0x81, REG, meta_group1, O(Ev, Iz)),
	Split(0x82, REG, meta_group1, O(Eb, Ib), F(I64)),
	Split(0x83, REG, meta_group1, O(Ev, Ib)),

	Opcode(0x84, M(TEST), O(Eb, Gb)),
	Opcode(0x85, M(TEST), O(Ev, Gv)),

	Opcode(0x88, M(MOV), O(Eb, Gb)),
	Opcode(0x89, M(MOV), O(Ev, Gv)),
	Opcode(0x8a, M(MOV), O(Gb, Eb)),
	Opcode(0x8b, M(MOV), O(Gv, Ev)),

	Opcode(0x8d, M(LEA), O(Gv, Mv)),

	Opcode(0x90, M(NOP)),

	Opcode(0x99, M(CWD), F(PRMO)),

	Opcode(0xa4, M(MOVS), O(Yb, Xb)),

	Opcode(0xa8, M(TEST), O(AL, Ib)),
	Opcode(0xa9, M(TEST), O(RA, Iz)),

	Opcode(0xb0, M(MOV), O(OPb, Ib)),
	Opcode(0xb1, M(MOV), O(OPb, Ib)),
	Opcode(0xb2, M(MOV), O(OPb, Ib)),
	Opcode(0xb3, M(MOV), O(OPb, Ib)),
	Opcode(0xb4, M(MOV), O(OPb, Ib)),
	Opcode(0xb5, M(MOV), O(OPb, Ib)),
	Opcode(0xb6, M(MOV), O(OPb, Ib)),
	Opcode(0xb7, M(MOV), O(OPb, Ib)),

	Opcode(0xb8, M(MOV), O(OPv, Iv)),
	Opcode(0xb9, M(MOV), O(OPv, Iv)),
	Opcode(0xba, M(MOV), O(OPv, Iv)),
	Opcode(0xbb, M(MOV), O(OPv, Iv)),
	Opcode(0xbc, M(MOV), O(OPv, Iv)),
	Opcode(0xbd, M(MOV), O(OPv, Iv)),
	Opcode(0xbe, M(MOV), O(OPv, Iv)),
	Opcode(0xbf, M(MOV), O(OPv, Iv)),

	Split(0xc0, REG, meta_group2, O(Eb, Ib)),
	Split(0xc1, REG, meta_group2, O(Ev, Ib)),

	Opcode(0xc2, M(RET), O(Iw), F(D64)),
	Opcode(0xc3, M(RET), F(D64)),

	Split(0xc6, REG, (OpcodeSplitReg)
	{
		Opcode(0b000, M(MOV), O(Eb, Ib)),
		Split(0b111, MOD, (OpcodeSplitMod)
		{
			Split(SPLIT_MOD_REG, RM, (OpcodeSplitRm)
			{
				Opcode(0b000, M(XABORT), O(Ib)),
			}),
		}),
	}),
	Split(0xc7, REG, (OpcodeSplitReg)
	{
		Opcode(0b000, M(MOV), O(Ev, Iz)),
		Split(0b111, MOD, (OpcodeSplitMod)
		{
			Split(SPLIT_MOD_REG, RM, (OpcodeSplitRm)
			{
				Opcode(0b000, M(XBEGIN), O(Jz)),
			}),
		}),
	}),

	Split(0xd1, REG, meta_group2, O(Eb, ONE)),
	Split(0xd2, REG, meta_group2, O(Ev, ONE)),
	Split(0xd3, REG, meta_group2, O(Eb, CL)),
	Split(0xd4, REG, meta_group2, O(Ev, CL)),

	Opcode(0xe8, M(CALL), O(Jz), F(F64)),
	Opcode(0xe9, M(JMP),  O(Jz), F(F64)),
	Opcode(0xea, M(JMP),  O(Ap), F(I64)),
	Opcode(0xeb, M(JMP),  O(Jb), F(F64)),

	Opcode(0xf4, M(HLT)),

	Split(0xf6, REG, meta_group3_f6, O(Eb)),
	Split(0xf7, REG, meta_group3_f7, O(Ev)),

	Opcode(0xf8, M(CLC)),
	Opcode(0xf9, M(STC)),
	Opcode(0xfa, M(CLI)),
	Opcode(0xfb, M(STI)),
	Opcode(0xfc, M(CLD)),
	Opcode(0xfd, M(STD)),

	Split(0xfe, REG, meta_group4),
	Split(0xff, REG, meta_group5),
};

CONST OpcodeSplitPfx	meta_base_0f_1e;
CONST OpcodeSplitReg	meta_base_0f_1f;
CONST OpcodeSplitPfx	meta_base_0f_58;

CONST OpcodeTable
meta_base_0f =
{
	Split(0x1e, PFX, (OpcodeSplitPfx)
	{
		Split(SPLIT_PFX_F3, MOD, (OpcodeSplitMod)
		{
			Split(SPLIT_MOD_REG, REG, (OpcodeSplitReg)
			{
				Split(0b111, RM, (OpcodeSplitRm)
				{
					Opcode(0b010, M(ENDBR64)),
					Opcode(0b011, M(ENDBR32)),
				}),
			}),
		}),
	}),
	Split(0x1f, REG, (OpcodeSplitReg)
	{
		Opcode(0b000, M(NOP), O(Ev)),
	}),

	Table(0x38, meta_base_0f_38),
	Table(0x3a, meta_base_0f_3a),

	Opcode(0x40, M(CMOVO),  O(Gv, Ev)),
	Opcode(0x41, M(CMOVNO), O(Gv, Ev)),
	Opcode(0x42, M(CMOVB),  O(Gv, Ev)),
	Opcode(0x43, M(CMOVAE), O(Gv, Ev)),
	Opcode(0x44, M(CMOVE),  O(Gv, Ev)),
	Opcode(0x45, M(CMOVNE), O(Gv, Ev)),
	Opcode(0x46, M(CMOVBE), O(Gv, Ev)),
	Opcode(0x47, M(CMOVA),  O(Gv, Ev)),
	Opcode(0x48, M(CMOVS),  O(Gv, Ev)),
	Opcode(0x49, M(CMOVNS), O(Gv, Ev)),
	Opcode(0x4a, M(CMOVP),  O(Gv, Ev)),
	Opcode(0x4b, M(CMOVNP), O(Gv, Ev)),
	Opcode(0x4c, M(CMOVL),  O(Gv, Ev)),
	Opcode(0x4d, M(CMOVGE), O(Gv, Ev)),
	Opcode(0x4e, M(CMOVLE), O(Gv, Ev)),
	Opcode(0x4f, M(CMOVG),  O(Gv, Ev)),

	Split(0x58, PFX, (OpcodeSplitPfx)
	{
		Opcode(SPLIT_PFX_NONE, M(ADDPS), O(Vps, Wps)),
		Opcode(SPLIT_PFX_NONE, M(ADDPD), O(Vpd, Wpd)),
		Opcode(SPLIT_PFX_NONE, M(ADDSS), O(Vss, Wss)),
		Opcode(SPLIT_PFX_NONE, M(ADDSD), O(Vsd, Wsd)),
	}),

	Opcode(0x80, M(JO),  O(Iz)),
	Opcode(0x81, M(JNO), O(Iz)),
	Opcode(0x82, M(JB),  O(Iz)),
	Opcode(0x83, M(JAE), O(Iz)),
	Opcode(0x84, M(JE),  O(Iz)),
	Opcode(0x85, M(JNE), O(Iz)),
	Opcode(0x86, M(JBE), O(Iz)),
	Opcode(0x87, M(JA),  O(Iz)),
	Opcode(0x88, M(JS),  O(Iz)),
	Opcode(0x89, M(JNS), O(Iz)),
	Opcode(0x8a, M(JP),  O(Iz)),
	Opcode(0x8b, M(JNP), O(Iz)),
	Opcode(0x8c, M(JL),  O(Iz)),
	Opcode(0x8d, M(JGE), O(Iz)),
	Opcode(0x8e, M(JLE), O(Iz)),
	Opcode(0x8f, M(JG),  O(Iz)),
	
	Opcode(0x90, M(SETO),  O(Eb)),
	Opcode(0x91, M(SETNO), O(Eb)),
	Opcode(0x92, M(SETB),  O(Eb)),
	Opcode(0x93, M(SETAE), O(Eb)),
	Opcode(0x94, M(SETE),  O(Eb)),
	Opcode(0x95, M(SETNE), O(Eb)),
	Opcode(0x96, M(SETBE), O(Eb)),
	Opcode(0x97, M(SETA),  O(Eb)),
	Opcode(0x98, M(SETS),  O(Eb)),
	Opcode(0x99, M(SETNS), O(Eb)),
	Opcode(0x9a, M(SETP),  O(Eb)),
	Opcode(0x9b, M(SETNP), O(Eb)),
	Opcode(0x9c, M(SETL),  O(Eb)),
	Opcode(0x9d, M(SETGE), O(Eb)),
	Opcode(0x9e, M(SETLE), O(Eb)),
	Opcode(0x9f, M(SETG),  O(Eb)),

	Opcode(0xb6, M(MOVZX), O(Gv, Eb)),
	Opcode(0xb7, M(MOVZX), O(Gv, Ew)),
};

CONST OpcodeSplitPfx	meta_base_0f_38_0x;

// 3 BYTES OPCODES (0F 38 XX)
CONST OpcodeTable
meta_base_0f_38 =
{
	Split(0x00, PFX, meta_base_0f_38_0x, M(PSHUFB)),
	Split(0x01, PFX, meta_base_0f_38_0x, M(PHADDW)),
	Split(0x02, PFX, meta_base_0f_38_0x, M(PHADDD)),
	Split(0x03, PFX, meta_base_0f_38_0x, M(PHADDSW)),
	Split(0x04, PFX, meta_base_0f_38_0x, M(PMADDUBSW)),
	Split(0x05, PFX, meta_base_0f_38_0x, M(PHSUBW)),
	Split(0x06, PFX, meta_base_0f_38_0x, M(PHSUBD)),
	Split(0x07, PFX, meta_base_0f_38_0x, M(PHSUBSW)),
	Split(0x08, PFX, meta_base_0f_38_0x, M(PSIGNB)),
	Split(0x09, PFX, meta_base_0f_38_0x, M(PSIGNW)),
	Split(0x0a, PFX, meta_base_0f_38_0x, M(PSIGND)),
	Split(0x0b, PFX, meta_base_0f_38_0x, M(PMULHRSW)),
};

// 3 BYTES OPCODES (0F 3A XX)
CONST OpcodeTable
meta_base_0f_3a =
{

};

CONST OpcodeSplitPfx
meta_base_0f_38_0x =
{
	Opcode(SPLIT_PFX_NONE, O(Pq, Qq)),
	Opcode(SPLIT_PFX_66,   O(Vx, Wx)),
};

CONST OpcodeSplitReg
meta_group1 =
{
	Opcode(0b000, M(ADD)),
	Opcode(0b001, M(OR)),
	Opcode(0b010, M(ADC)),
	Opcode(0b011, M(SBB)),
	Opcode(0b100, M(AND)),
	Opcode(0b101, M(SUB)),
	Opcode(0b110, M(XOR)),
	Opcode(0b111, M(CMP)),
};

CONST OpcodeSplitReg
meta_group2 =
{
	Opcode(0b000, M(ROL)),
	Opcode(0b001, M(ROR)),
	Opcode(0b010, M(RCL)),
	Opcode(0b011, M(RCR)),
	Opcode(0b100, M(SHL)),
	Opcode(0b101, M(SHR)),
	
	Opcode(0b111, M(SAR)),
};



#endif
