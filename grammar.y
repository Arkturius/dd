/**
* (PREFIX*)? (REX)? (OPCODE) (MODRM (SIB)? (DISP)?)? (IMM)?
* @@@@@@@@@@ @@@@@@ @@@@@@@@ @@@@@@ @@@@@@ @@@@@@@@@ @@@@@@ op0
* @@@@@@@@@@ ...... ........ ...... ...... ......... ...... prefix
* .......... @@@@@@ @@@@@@@@ @@@@@@ @@@@@@ @@@@@@@@@ @@@@@@ op1
* .......... @@@@@@ ........ ...... ...... ......... ...... rex
* .......... ...... @@@@@@@@ @@@@@@ @@@@@@ @@@@@@@@@ @@@@@@ op2
*/

op0
    : prefix_list op1
    ;

prefix_list
    : prefix
    | prefix prefix_list
    ;

prefix
    : lock_repeat_prefix
    | segment_jump_prefix
    | operand_size_prefix
    | address_size_prefix
    ;

lock_repeat_prefix
    : 0xF0 // LOCK
    | 0xF2 // REPNZ
    | 0xF3 // REPZ
    ;

// segment override prefixes
segment_jump_prefix
    : 0x26 // ES
    | 0x2E // CS - JCC: Branch not taken
    | 0x36 // SS
    | 0x3E // DS - JCC: Branch taken
    | 0x64 // FS
    | 0x65 // GS
    ;

operand_size_prefix
	: 0x66 // Mandatory on some instructions
	;

address_size_prefix
	: 0x67
	;

op1
	: rex op2
	| op2
	;

rex
	: 0x40 - 0x4F
	;

op2
	: opcode modifiers immediate
	| opcode modifiers
	| opcode immediate
	| opcode
	;


// mod:
//   mod == 11?    -> goto imm      // register-direct, no memory operand at all
//   rm == 100?    -> goto sib      // memory operand via SIB
//   else          -> goto disp     // memory operand, direct disp8/32
// 
// sib:
//   sib.base == 101 && mod == 00?  -> goto disp   // disp32, no base reg
//   else                             -> goto imm   // no additional displacement
// 
// disp:
//   parse disp (size from mod bits, or forced disp32 if via SIB.base==101 special case)
//   -> goto imm
// 
// imm:
//   parse optional(imm)  // size/presence from opcode table
