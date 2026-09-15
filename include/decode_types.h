/**
 * decode_types.h
 *
 * Raw encoding types consumed during decoding.
 */

#if !defined (_DECODE_TYPES_H)
# define _DECODE_TYPES_H

# include <basics.h>
# include <bitset.h>

typedef enum DecodePrefix
{
	PREFIX_LOCK,
	PREFIX_REPNE,
	PREFIX_REPE,
	PREFIX_SEG_ES,
	PREFIX_SEG_CS,
	PREFIX_SEG_SS,
	PREFIX_SEG_DS,
	PREFIX_SEG_FS,
	PREFIX_SEG_GS,
	PREFIX_OPSIZE,
	PREFIX_ADDRSIZE,
	enum_count(DecodePrefix),
}
DecodePrefix;
enum_check(DecodePrefix, 11);

static const u8
legacy_prefix_bytes[enum_count(DecodePrefix)] =
{
	[PREFIX_LOCK]     = 0xF0,
	[PREFIX_REPNE]    = 0xF2,
	[PREFIX_REPE]     = 0xF3,
	[PREFIX_SEG_ES]   = 0x26,
	[PREFIX_SEG_CS]   = 0x2E,
	[PREFIX_SEG_SS]   = 0x36,
	[PREFIX_SEG_DS]   = 0x3E,
	[PREFIX_SEG_FS]   = 0x64,
	[PREFIX_SEG_GS]   = 0x65,
	[PREFIX_OPSIZE]   = 0x66,
	[PREFIX_ADDRSIZE] = 0x67,
};

static const Bitset
legacy_prefix_map =
{
	.bits = 
	{
		0x4040404000000000,
		0x000000f000000000,
		0x0000000000000000,
		0x000d000000000000,
	}
};

# define	REPEAT_NONE	0
# define	REPEAT_E	1
# define	REPEAT_NE	2

# define	SEGMENT_ES	0
# define	SEGMENT_CS	1
# define	SEGMENT_SS	2
# define	SEGMENT_DS	3
# define	SEGMENT_FS	4
# define	SEGMENT_GS	5

typedef struct DecodePrefixState
{
	u8	lock     : 1;
	u8	repeat   : 2;
	u8	segment  : 3;
	u8	addrsize : 1;
	u8	opsize   : 1;
}
DecodePrefixState;

typedef union DecodeRex
{
	struct
	{
		u8	B : 1;
		u8	X : 1;
		u8	R : 1;
		u8	W : 1;
		u8	reserved : 4;
	};
	u8	raw;
}
DecodeRex;

// TODO: VEX, EVEX

typedef union DecodeVEX2
{
	struct
	{
		u8	id;
		u8	p : 2;
		u8	l : 1;
		u8	v : 4;
		u8	r : 1;
	};
	u8	raw[2];
}
DecodeVEX2;

typedef union DecodeVEX3
{
	struct
	{
		u8	id;
		u8	m : 5;
		u8	B : 1;
		u8	X : 1;
		u8	R : 1;
		u8	p : 2;
		u8	l : 1;
		u8	v : 4;
		u8	W : 1;
	};
	u8	raw[3];
}
DecodeVEX3;

typedef union DecodeEVEX
{
	struct
	{
		u8	id;
		u8	m    : 3;
		u8	zero : 1;
		u8	RR   : 1;
		u8	B    : 1;
		u8	X    : 1;
		u8	R    : 1;
		u8	p    : 2;
		u8	one  : 1;
		u8	v    : 4;
		u8	W    : 1;
		u8	a    : 3;
		u8	VV	 : 1;
		u8	b    : 1;
		u8	L    : 1;
		u8	LL   : 1;
		u8	z    : 1;
	};
	u8	raw[4];
}
DecodeEVEX;

typedef union DecodeExtension
{
	DecodeRex	rex;
	DecodeVEX2	vex2;
	DecodeVEX3	vex3;
	DecodeEVEX	evex;
	u8			raw[4];
}
DecodeExtension;

typedef union DecodeModRM
{
	struct
	{
		u8	rm  : 3;
		u8	reg : 3;
		u8	mod : 2;
	};
	u8	raw;
}
DecodeModRM;

typedef union DecodeSib
{
	struct
	{
		u8	base  : 3;
		u8	index : 3;
		u8	scale : 2;
	};
	u8	raw;
}
DecodeSib;

typedef DecodeSib	DecodeVSib;

typedef struct DecodeDisp
{
	u8	size;
	u64	disp;
}
DecodeDisp;

typedef struct DecodeImmediate
{
	u8	size;
	u64	imm;
}
DecodeImmediate;

typedef struct DecodeOpcode
{
	u8	len;
	u8	raw[15];
}
DecodeOpcode;

typedef enum DecodeAddressingMethod
{
	METHOD_0,
	METHOD_A,
	METHOD_B,
	METHOD_C,
	METHOD_D,
	METHOD_E,
	METHOD_F,
	METHOD_G,
	METHOD_H,
	METHOD_I,
	METHOD_J,
	METHOD_L,
	METHOD_M,
	METHOD_N,
	METHOD_O,
	METHOD_P,
	METHOD_Q,
	METHOD_R,
	METHOD_S,
	METHOD_U,
	METHOD_V,
	METHOD_W,
	METHOD_X,
	METHOD_Y,
	METHOD_OP,
	enum_count(DecodeAddressingMethod),
}
DecodeAddressingMethod;
enum_check(DecodeAddressingMethod, 25);
enum_backtype(DecodeAddressingMethod, u8);

typedef enum DecodeOperandType
{
	OPTYPE_0,
	OPTYPE_A,
	OPTYPE_B,
	OPTYPE_C,
	OPTYPE_D,
	OPTYPE_DQ,
	OPTYPE_P,
	OPTYPE_PD,
	OPTYPE_PI,
	OPTYPE_PS,
	OPTYPE_Q,
	OPTYPE_QQ,
	OPTYPE_S,
	OPTYPE_SD,
	OPTYPE_SS,
	OPTYPE_SI,
	OPTYPE_V,
	OPTYPE_W,
	OPTYPE_X,
	OPTYPE_Y,
	OPTYPE_Z,
	enum_count(DecodeOperandType),
}
DecodeOperandType;
enum_check(DecodeOperandType, 21);
enum_backtype(DecodeOperandType, u8);

#endif // _DECODE_TYPES_H
