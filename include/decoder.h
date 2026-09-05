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
	ENUM_COUNT(x86_OpcodeMap),
}
x86_OpcodeMap; ENUM_CHECK(x86_OpcodeMap,   4);

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
	ENUM_COUNT(x86_SegOverride),
}
x86_SegOverride; ENUM_CHECK(x86_SegOverride, 7);

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
	ENUM_COUNT(x86_Repeat),
}
x86_Repeat; ENUM_CHECK(x86_Repeat,      3);

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
	ENUM_COUNT(DecoderError),
}
DecoderError; ENUM_CHECK(DecoderError,    5);

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
	STATE_REX,
	STATE_PREFIX,
	STATE_OPCODE,
	STATE_ERROR,
	ENUM_COUNT(DecoderState),
}
DecoderState; ENUM_CHECK(DecoderState, 5);

typedef enum DecoderByteClass
{
	CLASS_NULL,
	CLASS_NORMAL,
	CLASS_REX,
	CLASS_PREFIX,
	ENUM_COUNT(DecoderByteClass),
}
DecoderByteClass; ENUM_CHECK(DecoderByteClass, 4);

typedef struct Decoder
{
	u64					pc;
	DecoderState		state;
	DecoderByteClass	bclass;
	x86_Instruction		ins;
}
Decoder;

static const DecoderByteClass	
byte_classes[0x100] =
{
	[0x00 ... 0xFF] = CLASS_NORMAL,
	[0x40 ... 0x4F] = CLASS_REX,
	[0xF0]          = CLASS_PREFIX,
	[0xF2]          = CLASS_PREFIX,
	[0xF3]          = CLASS_PREFIX,
	[0x26]          = CLASS_PREFIX,
	[0x2E]          = CLASS_PREFIX,
	[0x36]          = CLASS_PREFIX,
	[0x3E]          = CLASS_PREFIX,
	[0x64]          = CLASS_PREFIX,
	[0x65]          = CLASS_PREFIX,
	[0x66]          = CLASS_PREFIX,
	[0x67]          = CLASS_PREFIX,
};

# define	T(_current, _byteclass, _new)	[_current][_byteclass] = _new

static const DecoderState	
decoder_transitions[ENUM_COUNT(DecoderState)][ENUM_COUNT(DecoderByteClass)] =
{
	T(STATE_NULL, CLASS_NORMAL, STATE_OPCODE),
	T(STATE_NULL, CLASS_REX,    STATE_REX   ),
	T(STATE_NULL, CLASS_PREFIX, STATE_PREFIX),

	T(STATE_REX,  CLASS_NORMAL, STATE_OPCODE),
	T(STATE_REX,  CLASS_REX,    STATE_ERROR ),
	T(STATE_REX,  CLASS_PREFIX, STATE_OPCODE),

	T(STATE_PREFIX, CLASS_NORMAL, STATE_OPCODE),
	T(STATE_PREFIX, CLASS_REX,    STATE_REX   ),
	T(STATE_PREFIX, CLASS_PREFIX, STATE_PREFIX),

	T(STATE_OPCODE, CLASS_NORMAL, STATE_OPCODE),
	T(STATE_OPCODE, CLASS_REX,    STATE_OPCODE),
	T(STATE_OPCODE, CLASS_PREFIX, STATE_OPCODE),
};

typedef void	(*DecoderFunc)(Decoder *ins, Byte b);

void
decode_finalize(Decoder *d, Byte b);

void
decode_rex_prefix(Decoder *d, Byte b);

void
decode_raw_prefix(Decoder *d, Byte b);

void
decode_opcode(Decoder *d, Byte b);

void
decode_error(Decoder *d, Byte b);

static const DecoderFunc
decode_handlers[ENUM_COUNT(DecoderState)] =
{
	[STATE_NULL]   = decode_finalize,
	[STATE_REX]    = decode_rex_prefix,
	[STATE_PREFIX] = decode_raw_prefix,
	[STATE_OPCODE] = decode_opcode,
	[STATE_ERROR]  = decode_error,
};

/**
 
  STATE_NULL:
      case (CLASS_NORMAL) -> direct opcode -> STATE_OPCODE
	  case (CLASS_REX)    -> REX prefix    -> STATE_REX
	  case (CLASS_PREFIX) -> normal prefix -> STATE_PREFIX

  STATE_REX:
	  case (CLASS_NORMAL) -> opcode after REX    -> STATE_OPCODE
	  case (CLASS_REX)    -> REX after REX       -> STATE_ERROR
	  case (CLASS_PREFIX) -> !prefix because REX -> STATE_OPCODE

  STATE_PREFIX:
      case (CLASS_NORMAL) -> opcode after prefix(es) -> STATE_OPCODE
	  case (CLASS_REX)    -> REX after prefix(es)    -> STATE_REX
	  case (CLASS_PREFIX) -> normal prefix           -> STATE_PREFIX

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

DecoderError
decode(Bytes *code);

#endif // _DECODER_H
