BITS 64

section .text
global _start
_start:

operand_sizes:
	mov		al, bl   ; 8-bit,  no REX
	mov 	ax, bx   ; 16-bit, 0x66 prefix
	mov 	eax, ebx ; 32-bit, default
	mov 	rax, rbx ; 64-bit, REX.W
	
extended_registers:
	mov		r8b, r9b ; REX.RB
	mov 	r8d, r9d ; REX.RB, 32-bit
	mov 	r8, r9   ; REX.WRB, 64-bit
	mov 	spl, al  ; REX forces spl instead of legacy 'ah' encoding
	
immediate_sizes:
	mov		al, 0x12                ; imm8
	mov 	ax, 0x1234              ; imm16
	mov 	eax, 0x12345678         ; imm32
	mov 	rax, 0x123456789ABCDEF0 ; imm64 (mov r64,imm64)
	add 	rsp, 0x10               ; imm8 sx
	mov 	qword [rbx], 0x1234     ; C7 /0: r/m64, imm32 sx (forces REX.W+C7, not B8)
	
memory:
	mov		eax, [rbx] ; mod=00
	mov 	eax, [rbp] ; mod=00 (rbp requires disp8 = 0)
	mov 	eax, [r13] ; same with extended reg (REX.B + disp8 = 0)
	
disp8_vs_disp32:
	mov		eax, [rbx + 0x10]  ; disp8
	mov 	eax, [rbx + 0x100] ; disp32
	mov 	eax, [rbx - 0x10]  ; negative disp8
	mov 	eax, [rbx - 0x100] ; negative disp32
	
sib_byte:
	mov		eax, [rbx + rcx]              ; base + index, scale = 1, no disp
	mov 	eax, [rbx + rcx * 2]          ; scale = 2
	mov 	eax, [rbx + rcx * 4]          ; scale = 4
	mov 	eax, [rbx + rcx * 8]          ; scale = 8
	mov 	eax, [rbx + rcx * 4 + 0x10]   ; base + index * scale + disp8
	mov 	eax, [rbx + rcx * 4 + 0x1000] ; base + index * scale + disp32
	
sib_no_base:
	mov		eax, [rcx * 4 + 0x1000] ; index*scale + disp32 (mod=00, base=101)
	
sib_rsp_base:
	mov		eax, [rsp]        ; rsp cannot be encoded with ModRM, so SIB
	mov		eax, [rsp + 0x10] ; rsp + disp8
	
rip_relative:
	mov		eax, [rel label64] ; mod=00, rm=101 -> RIP-relative, disp32
	lea		rax, [rel label64]
	
segment_overrides:
	mov		eax, [fs: 0x28] ; common TLS/stack-cookie pattern
	mov		eax, [gs: 0x60]
	
opcode_sizes:
	nop                      ; single byte
	movzx	eax, byte [rbx]  ; 0Fh opcode map, size-extending load
	movzx	eax, word [rbx]
	movsx	rax, dword [rbx] ; 63h = MOVSXD, not 0Fh
	
mandatory_prefixes:
	addss	xmm0, xmm1      ; F3 0F opcode (SSE scalar single)
	addsd	xmm0, xmm1      ; F2 0F opcode (SSE scalar double)
	paddd	xmm0, xmm1      ; 66 0F opcode (SSE packed int)
	rep		movsb           ; F3 as REP, not mandatory here
	lock	add [rbx], eax  ; F0 LOCK prefix
	
no_modrm:
	push	rax  ; opcode encodes register, no ModRM
	pop		r15  ; REX.B + opcode + reg
	cdq          ; zero-operand
	ret          ; zero-operand
	
short_target:
	jmp		short short_target ; rel8
	jmp		near_target        ; rel32 (forced far by distance)
	call	near_target        ; rel32, call
	
three_byte_opcode:
	pshufb xmm0, xmm1 ; 66 0F 38 opcode map
	
disp_and_imm:
	mov dword [rbx+0x10], 0x1234 ; disp8 + imm32
	mov byte [rax], 0x7f         ; disp0 + imm8
	
	times 4096 nop

near_target:
	ret
	
label64:
	dq 0
