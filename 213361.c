static void scalar_min_max_and(struct bpf_reg_state *dst_reg,
			       struct bpf_reg_state *src_reg)
{
	s64 smin_val = src_reg->smin_value;
	u64 umax_val = src_reg->umax_value;

	/* We get our minimum from the var_off, since that's inherently
	 * bitwise.  Our maximum is the minimum of the operands' maxima.
	 */
	dst_reg->var_off = tnum_and(dst_reg->var_off, src_reg->var_off);
	dst_reg->umin_value = dst_reg->var_off.value;
	dst_reg->umax_value = min(dst_reg->umax_value, umax_val);
	if (dst_reg->smin_value < 0 || smin_val < 0) {
		/* Lose signed bounds when ANDing negative numbers,
		 * ain't nobody got time for that.
		 */
		dst_reg->smin_value = S64_MIN;
		dst_reg->smax_value = S64_MAX;
	} else {
		/* ANDing two positives gives a positive, so safe to
		 * cast result into s64.
		 */
		dst_reg->smin_value = dst_reg->umin_value;
		dst_reg->smax_value = dst_reg->umax_value;
	}
	/* We may learn something more from the var_off */
	__update_reg_bounds(dst_reg);
}