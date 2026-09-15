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

typedef struct Opcode
{
	x86_Mnemonic	mnemonic;
	OpcodeFlags		flags;
}
Opcode;

typedef struct OpcodeOperand
{
	DecodeAddressingMethod	method : 8;
	DecodeOperandType		type   : 8;
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
	SPLIT_PFX_F2,
	SPLIT_PFX_F3,
	enum_count(OpcodeMetaSplitPfx),
}
OpcodeMetaSplitPfx;
enum_check(OpcodeMetaSplitPfx, 4);

struct OpcodeMeta
{
	u8				kind;
	u8				nops;
	OpcodeOperand	ops[4];
	union
	{
		Opcode				opcode;
		const OpcodeMeta	*split;
	}	as;
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
	.kind     = META_TABLE,
	.as.split = opcode_meta_base,
};

static const OpcodeTable
opcode_meta_base =
{
	[0x00 ... ARRAY_SIZE(opcode_meta_base) - 1] = {0},

	[0x88] =
	{
		.kind      = META_OPCODE,
		.as.opcode =
		{
			.mnemonic = MNEMO_MOV,
			.flags    = {0},
		},
		.nops   = 2,
		.ops[0] = 
		{
			.method = METHOD_E,
			.type   = OPTYPE_B,
		},
		.ops[1] =
		{
			.method = METHOD_G,
			.type   = OPTYPE_B,
		},
	}
};

static const OpcodeTable
opcode_meta_0f =
{
	[0x00 ... ARRAY_SIZE(opcode_meta_0f) - 1] = {0},
};

static const OpcodeTable
opcode_meta_0f38 =
{
	[0x00 ... ARRAY_SIZE(opcode_meta_0f38) - 1] = {0},
};

static const OpcodeTable
opcode_meta_0f3a =
{
	[0x00 ... ARRAY_SIZE(opcode_meta_0f3a) - 1] = {0},
};

#endif // _OPCODES_H
