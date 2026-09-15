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
	MNEMO_MOV,
	MNEMO_ADD,
	enum_count(x86_Mnemonic),
}
x86_Mnemonic;
enum_check(x86_Mnemonic, 3);

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
}
x86_Instruction;

#endif // _X86_H
