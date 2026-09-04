/**
* decoder.h
*/

#if !defined (_DECODER_H)
# define _DECODER_H

#include <basics.h>

typedef u8		Byte;
typedef bool	x86_Flag;
typedef Byte	x86_Prefix;
typedef i32		x86_Offset;
typedef i64		x86_Immediate;

/**
 * @enum	x86_OpcodeMap
 *
 *	Different opcode maps depending on length
 */
typedef enum x86_OpcodeMap
{
	OPMAP_BASE,
	OPMAP_0F,
	OPMAP_0F38,
	OPMAP_0F3A,
	ENUM_GUARD(x86_OpcodeMap),
}
x86_OpcodeMap; ENUM_LEN_CHECK(x86_OpcodeMap,   4);

/**
 * @enum	x86_SegOverride
 *
 *	Segment override prefixes.
 */
typedef enum x86_SegOverride
{
	SEGMENT_NONE,
	SEGMENT_ES,
	SEGMENT_CS,
	SEGMENT_SS,
	SEGMENT_DS,
	SEGMENT_FS,
	SEGMENT_GS,
	ENUM_GUARD(x86_SegOverride),
}
x86_SegOverride; ENUM_LEN_CHECK(x86_SegOverride, 7);

/**
 * @enum	x86_Repeat
 *
 *	Repeat prefixes;
 */
typedef enum x86_Repeat
{
	REPEAT_NONE,
	REPEAT_F2,
	REPEAT_F3,
	ENUM_GUARD(x86_Repeat),
}
x86_Repeat; ENUM_LEN_CHECK(x86_Repeat,      3);

/**
 * @enum	DecoderError
 *
 *	Decoder error type.
 */
typedef enum DecoderError
{
	DECODE_OK,
	DECODE_TRUNCATED,
	DECODE_BAD_OPCODE,
	DECODE_BAD_PREFIX,
	DECODE_TOO_LONG,
	ENUM_GUARD(DecoderError),
}
DecoderError; ENUM_LEN_CHECK(DecoderError,    5);

typedef union x86_Rex
{
	struct
	{
		Byte	b : 1;
		Byte	x : 1;
		Byte	w : 1;
		Byte	r : 1;
		Byte	reserved : 4;
	};
	Byte	raw;
}
x86_Rex;

typedef union x86_ModRM
{
	struct
	{
		Byte	r_m : 3;
		Byte	reg : 3;
		Byte	mod : 2;
	};
	Byte	raw;
}
x86_ModRM;

typedef union x86_Sib
{
	struct
	{
		Byte	base  : 3;
		Byte	index : 3;
		Byte	scale : 2;
	};
	Byte	raw;
}
x86_Sib;

typedef struct x86_RawInstruction
{
	u8	count;
	u8	bytes[15];
}
x86_RawInstruction;

typedef struct x86_Instruction
{
	x86_RawInstruction	raw;

	x86_SegOverride	pre_segment;
	x86_Repeat		pre_repeat;
	x86_Flag		pre_lock;
	x86_Flag		pre_opsize;
	x86_Flag		pre_addrsize;
	x86_Rex			pre_rex;

	x86_OpcodeMap	opmap;
	Byte			opcode[3];
	u8				oplen;

	x86_Flag		has_modrm;
	x86_ModRM		modrm;

	x86_Flag		has_sib;
	x86_Sib			sib;

	x86_Flag		has_rip_disp;
	x86_Offset		disp;

	x86_Immediate	imm[2];
	u8				imm_sizes[2];
	u8				imm_count;

	u8				op_size;
	u8				addr_size;
	u8				disp_size;

	DecoderError	error;
}
x86_Instruction;

typedef enum DecoderState
{
	STATE_NULL,
	STATE_PREFIX,
	STATE_REX,
	STATE_OPCODE,
	STATE_ERROR,
	ENUM_GUARD(DecoderState),
}
DecoderState; ENUM_LEN_CHECK(DecoderState, 5);

typedef enum DecoderByteClass
{
	CLASS_NORMAL,
	CLASS_REX,
	CLASS_PREFIX,
	ENUM_GUARD(DecoderByteClass),
}
DecoderByteClass; ENUM_LEN_CHECK(DecoderByteClass, 3);

/**
 
  STATE_NULL:
      case (CLASS_NORMAL) -> direct opcode -> STATE_OPCODE
	  case (CLASS_REX)    -> REX prefix    -> STATE_REX
	  case (CLASS_PREFIX) -> normal prefix -> STATE_PREFIX

  STATE_PREFIX:
      case (CLASS_NORMAL) -> opcode after prefix(es) -> STATE_OPCODE
	  case (CLASS_REX)    -> REX after prefix(es)    -> STATE_REX
	  case (CLASS_PREFIX) -> normal prefix           -> STATE_PREFIX

  STATE_REX:
	  case (CLASS_NORMAL) -> opcode after REX    -> STATE_OPCODE
	  case (CLASS_REX)    -> REX after REX       -> STATE_ERROR
	  case (CLASS_PREFIX) -> !prefix because REX -> STATE_OPCODE

  STATE_OPCODE:
      handle opcode specifities:
	    decode REX if present
		decode MODRM if present
			decode SIB if present
			decode DISP if needed
		decode IMM if present
		canonicalize every info and push a DecoderOp

	typedef struct DecoderOp
	{
		// Everything that is needed to be dumped later
	}

 */

#endif // _DECODER_H
