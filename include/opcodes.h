/**
 * opcodes.h
 *
 * Defines Opcode metadata.
 */

#if !defined (_OPCODES_H)
# define _OPCODES_H

# include <basics.h>

# include <decode_types.h>
# include <x86.h>

# define	DFL_SIZE_RAW	0
# define	DFL_SIZE_D64	1
# define	DFL_SIZE_F64	2

# define	VALIDITY_RAW	0
# define	VALIDITY_I64	1
# define	VALIDITY_O64	2

typedef struct OpcodeFlags
{
	u8	default_size : 2;
	u8	validity     : 2;
	u8	lockable	 : 1;
	u8	privileged   : 1;
	u8	promotable   : 1;
}
OpcodeFlags;

typedef struct OpcodeOperand
{
	DecodeAddressingMethod	method : 8;
	union
	{
		DecodeOperandType	type   : 8;
		DecodeOperandFixed	fixed  : 8;
	};
}
OpcodeOperand;

typedef struct OpcodeMeta	OpcodeMeta;

typedef enum OpcodeMetaKind
{
	META_INVALID,
	META_OPCODE,
	META_TABLE,
	META_SPLIT_MOD,
	META_SPLIT_PFX,
	META_SPLIT_REG,
	META_SPLIT_RM,
	enum_count(OpcodeMetaKind),
}
OpcodeMetaKind;
enum_check(OpcodeMetaKind, 7);

typedef enum OpcodeMetaSplitMod
{
	SPLIT_MOD_MEM,
	SPLIT_MOD_REG,
	enum_count(OpcodeMetaSplitMod),
}
OpcodeMetaSplitMod;
enum_check(OpcodeMetaSplitMod, 2);

typedef enum OpcodeMetaSplitPfx
{
	SPLIT_PFX_NONE,
	SPLIT_PFX_66,
	SPLIT_PFX_F3,
	SPLIT_PFX_F2,
	enum_count(OpcodeMetaSplitPfx),
}
OpcodeMetaSplitPfx;
enum_check(OpcodeMetaSplitPfx, 4);

struct OpcodeMeta
{
	u8					kind;
	u8					nops;
	OpcodeOperand		ops[4];
	OpcodeFlags			flags;
	x86_Mnemonic		mnemonic;
	const OpcodeMeta	*split;
	const void			*_reserved; // maybe a function pointer for edge cases
									// bumps sizeof OpcodeMeta to 0x40 !
};

typedef OpcodeMeta	OpcodeTable    [0x100];
typedef OpcodeMeta	OpcodeSplitMod [enum_count(OpcodeMetaSplitMod)];
typedef OpcodeMeta	OpcodeSplitPfx [enum_count(OpcodeMetaSplitPfx)];
typedef OpcodeMeta	OpcodeSplitReg [0x08];
typedef OpcodeMeta	OpcodeSplitRm  [0x08];

// TODO: Imagine a DSL to write those in a pretty way

static const OpcodeTable	opcode_meta_base;
static const OpcodeMeta
opcode_meta_root =
{
	.kind  = META_TABLE,
	.split = opcode_meta_base,
};

# define	MOV_OPB_IB(_n)													\
	[0xb0 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_MOV,												\
		.nops     = 2,														\
		.ops[0]   = { .method = METHOD_OP, .type = OPTYPE_B },				\
		.ops[1]   = { .method = METHOD_I,  .type = OPTYPE_B },				\
	}

# define	MOV_OPV_IV(_n)													\
	[0xb8 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_MOV,												\
		.nops     = 2,														\
		.ops[0]   = { .method = METHOD_OP, .type = OPTYPE_V },				\
		.ops[1]   = { .method = METHOD_I,  .type = OPTYPE_V },				\
	}

# define	PUSH_GPR_64(_n)													\
	[0x50 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_PUSH,												\
		.nops     = 1,														\
		.ops[0]   = { .method = METHOD_OP, .type = OPTYPE_V },				\
		.flags    = { .default_size = DFL_SIZE_D64 },           			\
	}

# define	POP_GPR_64(_n)													\
	[0x58 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_POP,												\
		.nops     = 1,														\
		.ops      = {[0] = {.method = METHOD_OP, .type = OPTYPE_V }},		\
		.flags    = { .default_size = DFL_SIZE_D64 },           			\
	}

# define	JCC_IB_70_77(_n)												\
	[0x70 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_JO + _n,											\
		.nops     = 1,														\
		.ops      = {[0] = { .method = METHOD_J, .type = OPTYPE_B }},		\
		.flags    = { .default_size = DFL_SIZE_F64 },						\
	}

# define	JCC_IB_78_7F(_n)												\
	[0x78 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_JS + _n,											\
		.nops     = 1,														\
		.ops      = {[0] = { .method = METHOD_J, .type = OPTYPE_B }},		\
		.flags    = { .default_size = DFL_SIZE_F64 },						\
	}

# define	JCC_IZ_80_87(_n)												\
	[0x80 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_JO + _n,											\
		.nops     = 1,														\
		.ops      = {[0] = { .method = METHOD_J, .type = OPTYPE_Z }},		\
		.flags    = { .default_size = DFL_SIZE_F64 },						\
	}

# define	JCC_IZ_88_8F(_n)												\
	[0x88 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_JS + _n,											\
		.nops     = 1,														\
		.ops      = {[0] = { .method = METHOD_J, .type = OPTYPE_Z }},		\
		.flags    = { .default_size = DFL_SIZE_F64 },						\
	}

# define	SETCC_EB_90_97(_n)												\
	[0x90 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_SETO + _n,										\
		.nops     = 1,														\
		.ops      = {[0] = { .method = METHOD_E, .type = OPTYPE_B }},		\
		.flags    = { .default_size = DFL_SIZE_F64 },						\
	}

# define	SETCC_EB_98_9F(_n)												\
	[0x98 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_SETS + _n,										\
		.nops     = 1,														\
		.ops      = {[0] = { .method = METHOD_E, .type = OPTYPE_B }},		\
		.flags    = { .default_size = DFL_SIZE_F64 },						\
	}

# define	CMOVCC_GVEV_40_47(_n)											\
	[0x40 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_CMOVO + _n,										\
		.nops     = 2,														\
		.ops      =															\
		{																	\
			[0] = { .method = METHOD_G, .type = OPTYPE_V },					\
			[1] = { .method = METHOD_E, .type = OPTYPE_V },					\
		},																	\
	}

# define	CMOVCC_GVEV_48_4F(_n)											\
	[0x48 + _n] =															\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = MNEMO_CMOVS + _n,										\
		.nops     = 2,														\
		.ops      =															\
		{																	\
			[0] = { .method = METHOD_G, .type = OPTYPE_V },					\
			[1] = { .method = METHOD_E, .type = OPTYPE_V },					\
		},																	\
	}


# define	OPCODE_META_ROW(_ctor)	\
	_ctor (REG_ID_0),				\
	_ctor (REG_ID_1),				\
	_ctor (REG_ID_2),				\
	_ctor (REG_ID_3),				\
	_ctor (REG_ID_4),				\
	_ctor (REG_ID_5),				\
	_ctor (REG_ID_6),				\
	_ctor (REG_ID_7)

# define	OPCODE_ALU_CTOR(_n, _mn, _a0, _o0, _a1, _o1)					\
	[_n] =																	\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = _mn,													\
		.nops     = 2,														\
		.ops      =															\
		{																	\
			[0] = { .method = METHOD_ ## _a0 , .type = OPTYPE_ ## _o0 },	\
			[1] = { .method = METHOD_ ## _a1 , .type = OPTYPE_ ## _o1 },	\
		},																	\
	}

# define	OPCODE_ALU(_n, _mn)												\
	OPCODE_ALU_CTOR(_n,     _mn, E, B, G, B),								\
	OPCODE_ALU_CTOR(_n + 1, _mn, E, V, G, V),								\
	OPCODE_ALU_CTOR(_n + 2, _mn, G, B, E, B),								\
	OPCODE_ALU_CTOR(_n + 3, _mn, G, V, E, V),								\
	[_n + 4] =																\
	{																		\
		.kind     = META_OPCODE,											\
		.mnemonic = _mn,													\
		.nops     = 2,														\
		.ops      =															\
		{																	\
			[0] = { .method = METHOD_FX , .fixed = FX_AL    },				\
			[1] = { .method = METHOD_I  , .type  = OPTYPE_B },				\
		},																	\
	},								  										\
	[_n + 5] =																\
	{								  										\
		.kind     = META_OPCODE,											\
		.mnemonic = _mn,			  										\
		.nops     = 2,				  										\
		.ops      =					  										\
		{							  										\
			[0] = { .method = METHOD_FX , .fixed = FX_rAX   },				\
			[1] = { .method = METHOD_I  , .type  = OPTYPE_Z },				\
		},																	\
	}

static const OpcodeTable	opcode_meta_0f;
static const OpcodeTable	opcode_meta_0f38;
static const OpcodeTable	opcode_meta_0f3a;

static const OpcodeSplitPfx opcode_meta_0f_58;
static const OpcodeSplitReg opcode_meta_0f_1f;
static const OpcodeSplitPfx opcode_meta_0f_fe;

static const OpcodeSplitReg	opcode_meta_group1;
static const OpcodeSplitReg	opcode_meta_group2;
static const OpcodeSplitReg	opcode_meta_group3;
static const OpcodeSplitReg	opcode_meta_group5;
static const OpcodeSplitReg	opcode_meta_group11_c6;
static const OpcodeSplitReg	opcode_meta_group11_c7;

static const OpcodeTable
opcode_meta_base =
{
	[0x00 ... ARRAY_SIZE(opcode_meta_base) - 1] = {0},
	
	OPCODE_ALU(0x00, MNEMO_ADD),
	
	OPCODE_ALU(0x10, MNEMO_ADC),
	
	OPCODE_ALU(0x20, MNEMO_AND),
	
	OPCODE_ALU(0x30, MNEMO_XOR),

	OPCODE_ALU(0x08, MNEMO_OR),
	
	OPCODE_ALU(0x18, MNEMO_SBB),
	
	OPCODE_ALU(0x28, MNEMO_SUB),
	
	OPCODE_ALU(0x38, MNEMO_CMP),

	OPCODE_META_ROW(PUSH_GPR_64),
	OPCODE_META_ROW(POP_GPR_64),

	[0x63] = 
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_MOVSXD,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_G, .type = OPTYPE_V },
			[1] = { .method = METHOD_E, .type = OPTYPE_V },
		},
		.flags    = 
		{
			.promotable = true,
			.validity   = VALIDITY_O64
		},
	},

	OPCODE_META_ROW(JCC_IB_70_77),
	OPCODE_META_ROW(JCC_IB_78_7F),

	[0x80] = 
	{
		.kind   = META_SPLIT_REG,
		.split  = opcode_meta_group1,
		.nops   = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type = OPTYPE_B },
			[1] = { .method = METHOD_I, .type = OPTYPE_B },
		},
	},
	[0x81] = 
	{
		.kind   = META_SPLIT_REG,
		.split  = opcode_meta_group1,
		.nops   = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type = OPTYPE_V },
			[1] = { .method = METHOD_I, .type = OPTYPE_Z },
		},
	},
	[0x82] =
	{
		.kind  = META_SPLIT_REG,
		.split = opcode_meta_group1,
		.nops   = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type = OPTYPE_B },
			[1] = { .method = METHOD_I, .type = OPTYPE_B },
		},
		.flags = { .validity = VALIDITY_I64 },
	},
	[0x83] = 
	{
		.kind   = META_SPLIT_REG,
		.split  = opcode_meta_group1,
		.nops   = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type = OPTYPE_V },
			[1] = { .method = METHOD_I, .type = OPTYPE_B },
		},
	},

	[0x85] = 
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_TEST,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type = OPTYPE_V },
			[1] = { .method = METHOD_G, .type = OPTYPE_V },
		},
	},

	[0x88] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_MOV,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type = OPTYPE_B },
			[1] = { .method = METHOD_G, .type = OPTYPE_B },
		},
	},

	[0x89] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_MOV,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type = OPTYPE_V },
			[1] = { .method = METHOD_G, .type = OPTYPE_V },
		},
	},

	[0x8a] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_MOV,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_G, .type = OPTYPE_B },
			[1] = { .method = METHOD_E, .type = OPTYPE_B },
		},
	},

	[0x8b] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_MOV,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_G, .type = OPTYPE_V },
			[1] = { .method = METHOD_E, .type = OPTYPE_V },
		},
	},

	[0x8d] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_LEA,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_G, .type = OPTYPE_V },
			[1] = { .method = METHOD_M },
		},
	},

	[0x90] = 
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_NOP,
	},

	[0x99] = // RESOLVE MNEMONIC AT MATERIALIZE TIME BY ADDING 
			 // resolve_operand_size(dp, OPTYPE_V, meta) - SZ_WORD
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_CWD,
		.flags    = { .promotable = true },
	},

	[0xa4] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_MOVS,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_Y, .type = OPTYPE_B },
			[1] = { .method = METHOD_X, .type = OPTYPE_B },
		},
	},

	[0xa8] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_TEST,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_FX , .fixed = FX_AL    },
			[1] = { .method = METHOD_I  , .type  = OPTYPE_B },
		},
	},
	[0xa9] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_TEST,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_FX , .fixed = FX_rAX   },
			[1] = { .method = METHOD_I  , .type  = OPTYPE_Z },
		},
	},

	OPCODE_META_ROW(MOV_OPB_IB),
	OPCODE_META_ROW(MOV_OPV_IV),

	[0xc0] =
	{
		.kind     = META_SPLIT_REG,
		.split    = opcode_meta_group2,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type = OPTYPE_B },
			[1] = { .method = METHOD_I, .type = OPTYPE_B },
		},
	},
	[0xc1] =
	{
		.kind     = META_SPLIT_REG,
		.split    = opcode_meta_group2,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type = OPTYPE_V },
			[1] = { .method = METHOD_I, .type = OPTYPE_B },
		},
	},

	[0xc2] = 
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_RET,
		.nops     = 1,
		.ops      = {[0] = { .method = METHOD_I, .type = OPTYPE_W }},
		.flags    = { .default_size = DFL_SIZE_F64 },
	},
	[0xc3] = 
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_RET,
		.flags    = { .default_size = DFL_SIZE_F64 },
	},

	[0xc6] = 
	{
		.kind     = META_SPLIT_REG,
		.split    = opcode_meta_group11_c6,
	},
	[0xc7] = 
	{
		.kind     = META_SPLIT_REG,
		.split    = opcode_meta_group11_c7,
	},

	[0xd0] =
	{
		.kind     = META_SPLIT_REG,
		.split    = opcode_meta_group2,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E,  .type  = OPTYPE_B },
			[1] = { .method = METHOD_FX, .fixed = FX_1     },
		},
	},
	[0xd1] =
	{
		.kind     = META_SPLIT_REG,
		.split    = opcode_meta_group2,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E,  .type  = OPTYPE_V },
			[1] = { .method = METHOD_FX, .fixed = FX_1     },
		},
	},
	[0xd2] =
	{
		.kind     = META_SPLIT_REG,
		.split    = opcode_meta_group2,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E,  .type  = OPTYPE_B },
			[1] = { .method = METHOD_FX, .fixed = FX_CL    },
		},
	},
	[0xd3] =
	{
		.kind     = META_SPLIT_REG,
		.split    = opcode_meta_group2,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E,  .type  = OPTYPE_V },
			[1] = { .method = METHOD_FX, .fixed = FX_CL    },
		},
	},

	[0xe8] = 
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_CALL,
		.nops     = 1,
		.ops      = {[0] = { .method = METHOD_J, .type = OPTYPE_Z }},
		.flags    = { .default_size = DFL_SIZE_F64 },
	},
	[0xe9] = 
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_JMP,
		.nops     = 1,
		.ops      = {[0] = { .method = METHOD_J, .type = OPTYPE_Z }},
		.flags    = { .default_size = DFL_SIZE_F64 },
	},
	[0xea] =
	{
		.kind  = META_INVALID,
		.flags = { .validity = VALIDITY_I64 },
	},
	[0xeb] = 
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_JMP,
		.nops     = 1,
		.ops      = {[0] = { .method = METHOD_J, .type = OPTYPE_B }},
		.flags    = { .default_size = DFL_SIZE_F64 },
	},

	[0x0f] = 
	{
		.kind     = META_TABLE,
		.split    = opcode_meta_0f,
	},

	[0xf4] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_HLT,
	},

	[0xf6] = 
	{
		.kind  = META_SPLIT_REG,
		.split = opcode_meta_group3,
		.ops   = {[0] = { .method = METHOD_E, .type = OPTYPE_B }},

	},
	[0xf7] = 
	{
		.kind  = META_SPLIT_REG,
		.split = opcode_meta_group3,
		.ops   = {[0] = { .method = METHOD_E, .type = OPTYPE_V }},
	},

	[0xff] = 
	{
		.kind     = META_SPLIT_REG,
		.split    = opcode_meta_group5,
	},
};

static const OpcodeSplitPfx	opcode_meta_0f_1e;

static const OpcodeTable
opcode_meta_0f =
{
	[0x00 ... ARRAY_SIZE(opcode_meta_0f) - 1] = {0},

	[0x1e] =
	{
		.kind  = META_SPLIT_PFX,
		.split = opcode_meta_0f_1e,
	},

	[0x1f] =
	{
		.kind  = META_SPLIT_REG,
		.split = opcode_meta_0f_1f,
	},

	[0x58] =
	{
		.kind  = META_SPLIT_PFX,
		.split = opcode_meta_0f_58,
	},

	OPCODE_META_ROW(CMOVCC_GVEV_40_47),
	OPCODE_META_ROW(CMOVCC_GVEV_48_4F),

	OPCODE_META_ROW(JCC_IZ_80_87),
	OPCODE_META_ROW(JCC_IZ_88_8F),

	OPCODE_META_ROW(SETCC_EB_90_97),
	OPCODE_META_ROW(SETCC_EB_98_9F),

	[0xb6] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_MOVZX,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_G, .type = OPTYPE_V },
			[1] = { .method = METHOD_E, .type = OPTYPE_B },
		},
	},
	[0xb7] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_MOVZX,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_G, .type = OPTYPE_V },
			[1] = { .method = METHOD_E, .type = OPTYPE_W },
		},
	},

	[0x38] =
	{
		.kind  = META_TABLE,
		.split = opcode_meta_0f38,
	},

	[0xfe] =
	{
		.kind  = META_SPLIT_PFX,
		.split = opcode_meta_0f_fe,
	},
};

static const OpcodeSplitPfx
opcode_meta_0f38_0x =
{
	[SPLIT_PFX_NONE] =
	{
		.kind = META_OPCODE,
		.nops = 2,
		.ops  =
		{
			[0] = { .method = METHOD_P, .type = OPTYPE_Q },
			[1] = { .method = METHOD_Q, .type = OPTYPE_Q },
		},
	},
	[SPLIT_PFX_66] =
	{
		.kind = META_OPCODE,
		.nops = 2,
		.ops  =
		{
			[0] = { .method = METHOD_V, .type = OPTYPE_X },
			[1] = { .method = METHOD_W, .type = OPTYPE_X },
		},
	},
};

# define	SSE_0F38_0X(_n)				\
	[0x00 + _n] =					   	\
	{								   	\
		.kind     = META_SPLIT_PFX,	   	\
		.mnemonic = MNEMO_PSHUFB + _n,	\
		.split    = opcode_meta_0f38_0x \
	}

static const OpcodeTable
opcode_meta_0f38 =
{
	[0x00 ... ARRAY_SIZE(opcode_meta_0f38) - 1] = {0},

	OPCODE_META_ROW(SSE_0F38_0X),
};

static const OpcodeTable
opcode_meta_0f3a =
{
	[0x00 ... ARRAY_SIZE(opcode_meta_0f3a) - 1] = {0},
};

static const OpcodeSplitPfx
opcode_meta_0f_58 =
{
	[SPLIT_PFX_NONE] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_ADDPS,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_V, .type = OPTYPE_PS },
			[1] = { .method = METHOD_W, .type = OPTYPE_PS },
		},
	},
	[SPLIT_PFX_66] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_ADDPD,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_V, .type = OPTYPE_PD },
			[1] = { .method = METHOD_W, .type = OPTYPE_PD },
		},
	},
	[SPLIT_PFX_F3] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_ADDSS,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_V, .type = OPTYPE_SS },
			[1] = { .method = METHOD_W, .type = OPTYPE_SS },
		},
	},
	[SPLIT_PFX_F2] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_ADDSD,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_V, .type = OPTYPE_SD },
			[1] = { .method = METHOD_W, .type = OPTYPE_SD },
		},
	},
};

static const OpcodeSplitRm
opcode_meta_0f_1e_f3_11_111 =
{
	[0b010] = 
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_ENDBR64,
	},
	[0b011] = 
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_ENDBR32,
	},
};

static const OpcodeSplitReg
opcode_meta_0f_1e_f3_11 =
{
	[0b111] =
	{
		.kind = META_SPLIT_RM,
		.split = opcode_meta_0f_1e_f3_11_111,
	},
};

static const OpcodeSplitMod
opcode_meta_0f_1e_f3 =
{
	[SPLIT_MOD_REG] =
	{
		.kind  = META_SPLIT_REG,
		.split = opcode_meta_0f_1e_f3_11,
	},
};

static const OpcodeSplitPfx
opcode_meta_0f_1e =
{
	[SPLIT_PFX_F3] =
	{
		.kind  = META_SPLIT_MOD,
		.split = opcode_meta_0f_1e_f3
	},
};

static const OpcodeSplitReg
opcode_meta_0f_1f =
{
	[0b000] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_NOP,
		.nops     = 1,
		.ops      = {[0] = { .method = METHOD_E, .type = OPTYPE_V }}
	},
};

static const OpcodeSplitPfx
opcode_meta_0f_fe =
{
	[SPLIT_PFX_NONE] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_PADDD,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_P, .type = OPTYPE_Q },
			[1] = { .method = METHOD_Q, .type = OPTYPE_Q },
		},
	},
	[SPLIT_PFX_66] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_PADDD,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_V, .type = OPTYPE_X },
			[1] = { .method = METHOD_W, .type = OPTYPE_X },
		},
	},
};

static const OpcodeSplitReg
opcode_meta_group1 =
{
	[0b000] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_ADD,
	},
	[0b001] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_OR,
	},
	[0b010] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_ADC,
	},
	[0b011] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_SBB,
	},
	[0b100] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_AND,
	},
	[0b101] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_SUB,
	},
	[0b110] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_XOR,
	},
	[0b111] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_CMP,
	},
};

static const OpcodeSplitReg
opcode_meta_group2 =
{
	[0b101] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_SHR,
	},
	[0b100] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_SHL,
	},
	[0b111] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_SAR,
	},
};

static const OpcodeSplitReg
opcode_meta_group3 =
{
	
};

static const OpcodeSplitReg
opcode_meta_group5 =
{
	[0b010] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_CALL,
		.nops     = 1,
		.ops      = {[0] = { .method = METHOD_E, .type = OPTYPE_V }},
		.flags    = { .default_size = DFL_SIZE_F64 },
	},
	[0b100] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_JMP,
		.nops     = 1,
		.ops      = {[0] = { .method = METHOD_E, .type = OPTYPE_V }},
		.flags    = { .default_size = DFL_SIZE_F64 },
	},
};

static const OpcodeSplitRm
opcode_meta_group11_c7_111_11 =
{
	[0b000] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_XBEGIN,
		.nops     = 1,
		.ops      = {[0] = {.method = METHOD_J, .type  = OPTYPE_Z }},
	},
};

static const OpcodeSplitMod
opcode_meta_group11_c7_111 =
{
	[SPLIT_MOD_REG] =
	{
		.kind = META_SPLIT_RM,
		.split = opcode_meta_group11_c7_111_11,
	},
};

static const OpcodeSplitReg
opcode_meta_group11_c7 =
{
	[0b000] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_MOV,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type  = OPTYPE_V },
			[1] = { .method = METHOD_I, .type  = OPTYPE_Z },
		},
	},
	[0b111] =
	{
		.kind  = META_SPLIT_MOD,
		.split = opcode_meta_group11_c7_111
	},
};

static const OpcodeSplitRm
opcode_meta_group11_c6_111_11 =
{
	[0b000] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_XABORT,
		.nops     = 1,
		.ops      = {[0] = {.method = METHOD_I, .type  = OPTYPE_B }},
	},
};

static const OpcodeSplitMod
opcode_meta_group11_c6_111 =
{
	[SPLIT_MOD_REG] =
	{
		.kind = META_SPLIT_RM,
		.split = opcode_meta_group11_c6_111_11,
	},
};

static const OpcodeSplitReg
opcode_meta_group11_c6 =
{
	[0b000] =
	{
		.kind     = META_OPCODE,
		.mnemonic = MNEMO_MOV,
		.nops     = 2,
		.ops      =
		{
			[0] = { .method = METHOD_E, .type  = OPTYPE_B },
			[1] = { .method = METHOD_I, .type  = OPTYPE_B },
		},
	},
	[0b111] =
	{
		.kind  = META_SPLIT_MOD,
		.split = opcode_meta_group11_c6_111
	},
};

#endif // _OPCODES_H
