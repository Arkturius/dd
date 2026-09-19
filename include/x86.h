/**
 * x86.h
 *
 * Defines structures for x86 instructions
 */

#if !defined (_X86_H)
# define _X86_H

# include <basics.h>

typedef enum x86_OperandKind
{
	OPKIND_NONE,
	OPKIND_REGISTER,
	OPKIND_IMMEDIATE,
	OPKIND_MEMORY,
	enum_count(x86_OperandKind),
}
x86_OperandKind;
enum_check(x86_OperandKind, 4);

// TODO:
// needs const char * display
// needs u32          sizes
typedef enum x86_Size
{
	SZ_BYTE,
	SZ_WORD,
	SZ_DWORD,
	SZ_QWORD,
	SZ_XMMWORD,
	SZ_YMMWORD,
	SZ_ZMMWORD,
	SZ_FARPTR,
	SZ_TBYTE,
	enum_count(x86_Size),
}
x86_Size;
enum_check(x86_Size, 9);
static_assert(SZ_XMMWORD + 1 == SZ_YMMWORD);
static_assert(SZ_XMMWORD + 2 == SZ_ZMMWORD);

static const u32
x86_sizes_in_bits[enum_count(x86_Size)] =
{
	[SZ_BYTE]    = 8,
	[SZ_WORD]    = 16,
	[SZ_DWORD]   = 32,
	[SZ_QWORD]   = 64,
	[SZ_XMMWORD] = 128,
	[SZ_YMMWORD] = 256,
	[SZ_ZMMWORD] = 512,
	[SZ_FARPTR]  = 48,
	[SZ_TBYTE]   = 80,
};

// TODO:
// needs u8	register_counts
typedef enum x86_RegisterClass
{
	REG_CLASS_GPR,
	REG_CLASS_SEG,
	REG_CLASS_RIP,
	REG_CLASS_FLG,
	REG_CLASS_X87,
	REG_CLASS_MMX,
	REG_CLASS_VEC,
	REG_CLASS_MSK,
	REG_CLASS_BND,
	REG_CLASS_CTL,
	REG_CLASS_DBG,
	REG_CLASS_SYS,
	enum_count(x86_RegisterClass),
}
x86_RegisterClass;
enum_check(x86_RegisterClass, 12);

// TODO:
// needs a big const char **[CLASS]
//  and CLASS arrays of size register_counts[CLASS]
typedef enum x86_RegisterId
{
	REG_ID_0,
	REG_ID_1,
	REG_ID_2,
	REG_ID_3,
	REG_ID_4,
	REG_ID_5,
	REG_ID_6,
	REG_ID_7,
	REG_ID_8,
	REG_ID_9,
	REG_ID_10,
	REG_ID_11,
	REG_ID_12,
	REG_ID_13,
	REG_ID_14,
	REG_ID_15,
	REG_ID_16,
	REG_ID_17,
	REG_ID_18,
	REG_ID_19,
	REG_ID_20,
	REG_ID_21,
	REG_ID_22,
	REG_ID_23,
	REG_ID_24,
	REG_ID_25,
	REG_ID_26,
	REG_ID_27,
	REG_ID_28,
	REG_ID_29,
	REG_ID_30,
	REG_ID_31,
	enum_count(x86_RegisterId),
}
x86_RegisterId;
enum_check(x86_RegisterId, 32);

// TODO:
// needs const char *segment_names
typedef enum x86_Segment
{
	SEG_NONE,
	SEG_ES,
	SEG_CS,
	SEG_SS,
	SEG_DS,
	SEG_FS,
	SEG_GS,
	enum_count(x86_Segment),
}
x86_Segment;
enum_check(x86_Segment, 7);

typedef enum x86_Scale
{
	SCALE_BYTE,
	SCALE_WORD,
	SCALE_DWORD,
	SCALE_QWORD,
	enum_count(x86_Scale),
}
x86_Scale;
enum_check(x86_Scale, 4);

// TODO:
// needs const char *mnemonics
typedef enum x86_Mnemonic
{
	MNEMO_INVALID,
	MNEMO_NOP,
	MNEMO_ENDBR32,
	MNEMO_ENDBR64,
	MNEMO_ADD,
	MNEMO_OR,
	MNEMO_ADC,
	MNEMO_SBB,
	MNEMO_AND,
	MNEMO_SUB,
	MNEMO_XOR,
	MNEMO_CMP,
	MNEMO_TEST,
	MNEMO_ROL,
	MNEMO_ROR,
	MNEMO_RCL,
	MNEMO_RCR,
	MNEMO_SHL,
	MNEMO_SHR,
	MNEMO_SAR,
	MNEMO_MOV,
	MNEMO_MOVZX,
	MNEMO_MOVSX,
	MNEMO_MOVSXD,
	MNEMO_LEA,
	MNEMO_PUSH,
	MNEMO_POP,
	MNEMO_JMP,
	MNEMO_JO,
	MNEMO_JNO,
	MNEMO_JB,
	MNEMO_JAE,
	MNEMO_JE,
	MNEMO_JNE,
	MNEMO_JBE,
	MNEMO_JA,
	MNEMO_JS,
	MNEMO_JNS,
	MNEMO_JP,
	MNEMO_JNP,
	MNEMO_JL,
	MNEMO_JGE,
	MNEMO_JLE,
	MNEMO_JG,
	MNEMO_SETO,
	MNEMO_SETNO,
	MNEMO_SETB,
	MNEMO_SETAE,
	MNEMO_SETE,
	MNEMO_SETNE,
	MNEMO_SETBE,
	MNEMO_SETA,
	MNEMO_SETS,
	MNEMO_SETNS,
	MNEMO_SETP,
	MNEMO_SETNP,
	MNEMO_SETL,
	MNEMO_SETGE,
	MNEMO_SETLE,
	MNEMO_SETG,
	MNEMO_CMOVO,
	MNEMO_CMOVNO,
	MNEMO_CMOVB,
	MNEMO_CMOVAE,
	MNEMO_CMOVE,
	MNEMO_CMOVNE,
	MNEMO_CMOVBE,
	MNEMO_CMOVA,
	MNEMO_CMOVS,
	MNEMO_CMOVNS,
	MNEMO_CMOVP,
	MNEMO_CMOVNP,
	MNEMO_CMOVL,
	MNEMO_CMOVGE,
	MNEMO_CMOVLE,
	MNEMO_CMOVG,
	MNEMO_CALL,
	MNEMO_HLT,
	MNEMO_CLD,
	MNEMO_CLI,
	MNEMO_XABORT,
	MNEMO_XBEGIN,
	MNEMO_MOVS,
	MNEMO_CWD,
	MNEMO_CDQ,
	MNEMO_CQO,
	MNEMO_RET,
	MNEMO_PADDD,
	MNEMO_PSHUFB,
	MNEMO_PHADDW,
	MNEMO_PHADDD,
	MNEMO_PHADDSW,
	MNEMO_PMADDUBSW,
	MNEMO_PHSUBW,
	MNEMO_PHSUBD,
	MNEMO_PHSUBSW,
	MNEMO_ADDPS,
	MNEMO_ADDPD,
	MNEMO_ADDSS,
	MNEMO_ADDSD,
	enum_count(x86_Mnemonic),
}
x86_Mnemonic;
enum_check(x86_Mnemonic, 100);

static_assert(MNEMO_JO    + 1 == MNEMO_JNO);
static_assert(MNEMO_JNO   + 1 == MNEMO_JB);
static_assert(MNEMO_JB    + 1 == MNEMO_JAE);
static_assert(MNEMO_JAE   + 1 == MNEMO_JE);
static_assert(MNEMO_JE    + 1 == MNEMO_JNE);
static_assert(MNEMO_JNE   + 1 == MNEMO_JBE);
static_assert(MNEMO_JBE   + 1 == MNEMO_JA);
static_assert(MNEMO_JA    + 1 == MNEMO_JS);
static_assert(MNEMO_JS    + 1 == MNEMO_JNS);
static_assert(MNEMO_JNS   + 1 == MNEMO_JP);
static_assert(MNEMO_JP    + 1 == MNEMO_JNP);
static_assert(MNEMO_JNP   + 1 == MNEMO_JL);
static_assert(MNEMO_JL    + 1 == MNEMO_JGE);
static_assert(MNEMO_JGE   + 1 == MNEMO_JLE);
static_assert(MNEMO_JLE   + 1 == MNEMO_JG);

static_assert(MNEMO_SETO  + 1 == MNEMO_SETNO);
static_assert(MNEMO_SETNO + 1 == MNEMO_SETB);
static_assert(MNEMO_SETB  + 1 == MNEMO_SETAE);
static_assert(MNEMO_SETAE + 1 == MNEMO_SETE);
static_assert(MNEMO_SETE  + 1 == MNEMO_SETNE);
static_assert(MNEMO_SETNE + 1 == MNEMO_SETBE);
static_assert(MNEMO_SETBE + 1 == MNEMO_SETA);
static_assert(MNEMO_SETA  + 1 == MNEMO_SETS);
static_assert(MNEMO_SETS  + 1 == MNEMO_SETNS);
static_assert(MNEMO_SETNS + 1 == MNEMO_SETP);
static_assert(MNEMO_SETP  + 1 == MNEMO_SETNP);
static_assert(MNEMO_SETNP + 1 == MNEMO_SETL);
static_assert(MNEMO_SETL  + 1 == MNEMO_SETGE);
static_assert(MNEMO_SETGE + 1 == MNEMO_SETLE);
static_assert(MNEMO_SETLE + 1 == MNEMO_SETG);

static_assert(MNEMO_CMOVO  + 1 == MNEMO_CMOVNO);
static_assert(MNEMO_CMOVNO + 1 == MNEMO_CMOVB);
static_assert(MNEMO_CMOVB  + 1 == MNEMO_CMOVAE);
static_assert(MNEMO_CMOVAE + 1 == MNEMO_CMOVE);
static_assert(MNEMO_CMOVE  + 1 == MNEMO_CMOVNE);
static_assert(MNEMO_CMOVNE + 1 == MNEMO_CMOVBE);
static_assert(MNEMO_CMOVBE + 1 == MNEMO_CMOVA);
static_assert(MNEMO_CMOVA  + 1 == MNEMO_CMOVS);
static_assert(MNEMO_CMOVS  + 1 == MNEMO_CMOVNS);
static_assert(MNEMO_CMOVNS + 1 == MNEMO_CMOVP);
static_assert(MNEMO_CMOVP  + 1 == MNEMO_CMOVNP);
static_assert(MNEMO_CMOVNP + 1 == MNEMO_CMOVL);
static_assert(MNEMO_CMOVL  + 1 == MNEMO_CMOVGE);
static_assert(MNEMO_CMOVGE + 1 == MNEMO_CMOVLE);
static_assert(MNEMO_CMOVLE + 1 == MNEMO_CMOVG);

static const char
*x86_mnemonics[enum_count(x86_Mnemonic)] =
{
	[MNEMO_INVALID]   = "invalid",
	[MNEMO_NOP]       = "nop",
	[MNEMO_ENDBR32]   = "endbr32",
	[MNEMO_ENDBR64]   = "endbr64",
	[MNEMO_ADD]       = "add",
	[MNEMO_OR]        = "or",
	[MNEMO_ADC]       = "adc",
	[MNEMO_SBB]       = "sbb",
	[MNEMO_AND]       = "and",
	[MNEMO_SUB]       = "sub",
	[MNEMO_XOR]       = "xor",
	[MNEMO_CMP]       = "cmp",
	[MNEMO_TEST]      = "test",
	[MNEMO_ROL]       = "rol",
	[MNEMO_ROR]       = "ror",
	[MNEMO_RCL]       = "rcl",
	[MNEMO_RCR]       = "rcr",
	[MNEMO_SHL]       = "shl",
	[MNEMO_SHR]       = "shr",
	[MNEMO_SAR]       = "sar",
	[MNEMO_MOV]       = "mov",
	[MNEMO_MOVZX]     = "movzx",
	[MNEMO_MOVSX]     = "movsx",
	[MNEMO_MOVSXD]    = "movsxd",
	[MNEMO_LEA]       = "lea",
	[MNEMO_PUSH]      = "push",
	[MNEMO_POP]       = "pop",
	[MNEMO_JMP]       = "jmp",
	[MNEMO_JO]        = "jo",
	[MNEMO_JNO]       = "jno",
	[MNEMO_JB]        = "jb",
	[MNEMO_JAE]       = "jae",
	[MNEMO_JE]        = "je",
	[MNEMO_JNE]       = "jne",
	[MNEMO_JBE]       = "jbe",
	[MNEMO_JA]        = "ja",
	[MNEMO_JS]        = "js",
	[MNEMO_JNS]       = "jns",
	[MNEMO_JP]        = "jp",
	[MNEMO_JNP]       = "jnp",
	[MNEMO_JL]        = "jl",
	[MNEMO_JGE]       = "jge",
	[MNEMO_JLE]       = "jle",
	[MNEMO_JG]        = "jg",
	[MNEMO_SETO]      = "seto",
	[MNEMO_SETNO]     = "setno",
	[MNEMO_SETB]      = "setb",
	[MNEMO_SETAE]     = "setae",
	[MNEMO_SETE]      = "sete",
	[MNEMO_SETNE]     = "setne",
	[MNEMO_SETBE]     = "setbe",
	[MNEMO_SETA]      = "seta",
	[MNEMO_SETS]      = "sets",
	[MNEMO_SETNS]     = "setns",
	[MNEMO_SETP]      = "setp",
	[MNEMO_SETNP]     = "setnp",
	[MNEMO_SETL]      = "setl",
	[MNEMO_SETGE]     = "setge",
	[MNEMO_SETLE]     = "setle",
	[MNEMO_SETG]      = "setg",
	[MNEMO_CMOVO]     = "cmovo",
	[MNEMO_CMOVNO]    = "cmovno",
	[MNEMO_CMOVB]     = "cmovb",
	[MNEMO_CMOVAE]    = "cmovae",
	[MNEMO_CMOVE]     = "cmove",
	[MNEMO_CMOVNE]    = "cmovne",
	[MNEMO_CMOVBE]    = "cmovbe",
	[MNEMO_CMOVA]     = "cmova",
	[MNEMO_CMOVS]     = "cmovs",
	[MNEMO_CMOVNS]    = "cmovns",
	[MNEMO_CMOVP]     = "cmovp",
	[MNEMO_CMOVNP]    = "cmovnp",
	[MNEMO_CMOVL]     = "cmovl",
	[MNEMO_CMOVGE]    = "cmovge",
	[MNEMO_CMOVLE]    = "cmovle",
	[MNEMO_CMOVG]     = "cmovg",
	[MNEMO_CALL]      = "call",
	[MNEMO_HLT]       = "hlt",
	[MNEMO_CLD]       = "cld",
	[MNEMO_CLI]       = "cli",
	[MNEMO_XABORT]    = "xabort",
	[MNEMO_XBEGIN]    = "xbegin",
	[MNEMO_MOVS]      = "movs",
	[MNEMO_CWD]       = "cwd",
	[MNEMO_CDQ]       = "cdq",
	[MNEMO_CQO]       = "cqo",
	[MNEMO_RET]       = "ret",
	[MNEMO_PADDD]     = "paddd",
	[MNEMO_PSHUFB]    = "pshufb",
	[MNEMO_PHADDW]    = "phaddw",
	[MNEMO_PHADDD]    = "phaddd",
	[MNEMO_PHADDSW]   = "phaddsw",
	[MNEMO_PMADDUBSW] = "pmaddubsw",
	[MNEMO_PHSUBW]    = "phsubw",
	[MNEMO_PHSUBD]    = "phsubd",
	[MNEMO_PHSUBSW]   = "phsubsw",
	[MNEMO_ADDPS]     = "addps",
	[MNEMO_ADDPD]     = "addpd",
	[MNEMO_ADDSS]     = "addss",
	[MNEMO_ADDSD]     = "addsd",
};

static_assert(MNEMO_CWD + 1 == MNEMO_CDQ && MNEMO_CDQ + 1 == MNEMO_CQO);

typedef u8	x86_Flag;
typedef i64	x86_Immediate;
typedef i64	x86_Displacement;

typedef struct x86_Register
{
	x86_RegisterClass	c;
	x86_RegisterId		id;
	x86_Size			size;
	x86_Flag			hi8;
}
x86_Register;

typedef struct x86_Memory
{
	x86_Segment			seg;
	x86_Register		base;
	x86_Register		index;
	x86_Scale			scale;
	x86_Displacement	disp;
}
x86_Memory;

typedef struct x86_Operand
{
	x86_OperandKind	kind;
	x86_Size		size;
	union
	{
		x86_Immediate	imm;
		x86_Register	reg;
		x86_Memory		mem;
	}	as;
}
x86_Operand;

typedef struct x86_Attributes
{
	x86_Flag	lock       : 1;
	x86_Flag	repeat     : 2;
	x86_Flag	evex_maskz : 1;
	x86_Flag	evex_bcast : 1;
	x86_Flag	evex_rdsae : 1;
}
x86_Attributes;

typedef struct x86_Instruction
{
	x86_Mnemonic	mnemo;
	x86_Operand		ops[4];
	u32				len;
	x86_Attributes	attrs;
	void			*_pad[2];
}
x86_Instruction;

#endif // _X86_H
