/**
 * methods.c

void
decode_method_e(Decoder *d, x86_Operand *op, x86_OperandSize size)
{
	x86_InstructionInfo	*info = &d->op;

	op->size = size;
	if (info->modrm.mod == 0b11)
	{
		op->kind   = OPKIND_REG;
		op->as.reg = (x86_Register)info->modrm.r_m | (info->pre_rex.b << 3);
		return ;
	}
	decode_method_m(d, op, size);
}

void
decode_method_g(Decoder *d, x86_Operand *op, x86_OperandSize size)
{
	x86_InstructionInfo	*info = &d->op;

	op->kind   = OPKIND_REG;
	op->size   = size;
	op->as.reg = (x86_Register)info->modrm.reg | (info->pre_rex.r << 3);
}

void
decode_method_i(Decoder *d, x86_Operand *op, x86_OperandSize size)
{
	op->kind   = OPKIND_IMM;
	op->as.imm = decode_inline_value(d, size);
}

void
decode_method_m(Decoder *d, x86_Operand *op, x86_OperandSize size)
{
	x86_InstructionInfo	*info = &d->op;

	UNUSED(size);
	op->kind   = OPKIND_MEM;
	op->as.mem = (x86_Memory)
	{
		.base  = REG_NONE,
		.index = REG_NONE,
		.scale = SCALE_BYTE,
		.disp_size = OPSIZE_NONE,
		.disp  = 0,
	};
	if (info->modrm.mod == 0b01)
		op->as.mem.disp_size = OPSIZE_BYTE;
	if (info->modrm.mod == 0b10)
		op->as.mem.disp_size = OPSIZE_DWORD;
	if (info->has_sib)
	{
		op->as.mem.base  = info->sib.base;
		op->as.mem.base |= (info->pre_rex.b << 3);
		op->as.mem.index = info->sib.index;
		op->as.mem.scale = info->sib.scale;
		if (info->sib.base == 0b101 && info->modrm.mod == 0b00)
		{
			op->as.mem.disp_size = OPSIZE_DWORD;
			op->as.mem.base      = REG_NONE;
		}
		if (info->sib.index == 0b100)
			op->as.mem.index = REG_NONE;
	}
	else if (info->modrm.mod == 0b00 && info->modrm.r_m == 0b101)
	{
		op->as.mem.base      = REG_RIP;
		op->as.mem.disp_rip  = true;
		op->as.mem.disp_size = OPSIZE_DWORD;
	}
	else
		op->as.mem.base = info->modrm.r_m | (info->pre_rex.b << 3);
	if (op->as.mem.disp_size != OPSIZE_NONE)
		op->as.mem.disp = decode_inline_value(d, op->as.mem.disp_size);
}

void
decode_method_op(Decoder *d, x86_Operand *op, x86_OperandSize size)
{
	x86_InstructionInfo	*info = &d->op;

	op->kind    = OPKIND_REG;
	op->size    = size;
	op->as.reg  = (x86_Register)info->opcode[info->oplen - 1] & 0b111;
	op->as.reg |= info->pre_rex.b << 3;
}

 */
