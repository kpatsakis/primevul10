static int is_branch_taken(struct bpf_reg_state *reg, u64 val, u8 opcode,
			   bool is_jmp32)
{
	struct bpf_reg_state reg_lo;
	s64 sval;

	if (__is_pointer_value(false, reg))
		return -1;

	if (is_jmp32) {
		reg_lo = *reg;
		reg = &reg_lo;
		/* For JMP32, only low 32 bits are compared, coerce_reg_to_size
		 * could truncate high bits and update umin/umax according to
		 * information of low bits.
		 */
		coerce_reg_to_size(reg, 4);
		/* smin/smax need special handling. For example, after coerce,
		 * if smin_value is 0x00000000ffffffffLL, the value is -1 when
		 * used as operand to JMP32. It is a negative number from s32's
		 * point of view, while it is a positive number when seen as
		 * s64. The smin/smax are kept as s64, therefore, when used with
		 * JMP32, they need to be transformed into s32, then sign
		 * extended back to s64.
		 *
		 * Also, smin/smax were copied from umin/umax. If umin/umax has
		 * different sign bit, then min/max relationship doesn't
		 * maintain after casting into s32, for this case, set smin/smax
		 * to safest range.
		 */
		if ((reg->umax_value ^ reg->umin_value) &
		    (1ULL << 31)) {
			reg->smin_value = S32_MIN;
			reg->smax_value = S32_MAX;
		}
		reg->smin_value = (s64)(s32)reg->smin_value;
		reg->smax_value = (s64)(s32)reg->smax_value;

		val = (u32)val;
		sval = (s64)(s32)val;
	} else {
		sval = (s64)val;
	}

	switch (opcode) {
	case BPF_JEQ:
		if (tnum_is_const(reg->var_off))
			return !!tnum_equals_const(reg->var_off, val);
		break;
	case BPF_JNE:
		if (tnum_is_const(reg->var_off))
			return !tnum_equals_const(reg->var_off, val);
		break;
	case BPF_JSET:
		if ((~reg->var_off.mask & reg->var_off.value) & val)
			return 1;
		if (!((reg->var_off.mask | reg->var_off.value) & val))
			return 0;
		break;
	case BPF_JGT:
		if (reg->umin_value > val)
			return 1;
		else if (reg->umax_value <= val)
			return 0;
		break;
	case BPF_JSGT:
		if (reg->smin_value > sval)
			return 1;
		else if (reg->smax_value < sval)
			return 0;
		break;
	case BPF_JLT:
		if (reg->umax_value < val)
			return 1;
		else if (reg->umin_value >= val)
			return 0;
		break;
	case BPF_JSLT:
		if (reg->smax_value < sval)
			return 1;
		else if (reg->smin_value >= sval)
			return 0;
		break;
	case BPF_JGE:
		if (reg->umin_value >= val)
			return 1;
		else if (reg->umax_value < val)
			return 0;
		break;
	case BPF_JSGE:
		if (reg->smin_value >= sval)
			return 1;
		else if (reg->smax_value < sval)
			return 0;
		break;
	case BPF_JLE:
		if (reg->umax_value <= val)
			return 1;
		else if (reg->umin_value > val)
			return 0;
		break;
	case BPF_JSLE:
		if (reg->smax_value <= sval)
			return 1;
		else if (reg->smin_value > sval)
			return 0;
		break;
	}

	return -1;
}