static bool cmp_val_with_extended_s64(s64 sval, struct bpf_reg_state *reg)
{
	return ((s32)sval >= 0 &&
		reg->smin_value >= 0 && reg->smax_value <= S32_MAX) ||
	       ((s32)sval < 0 &&
		reg->smax_value <= 0 && reg->smin_value >= S32_MIN);
}