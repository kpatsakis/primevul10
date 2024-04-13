static bool reg_may_point_to_spin_lock(const struct bpf_reg_state *reg)
{
	return reg->type == PTR_TO_MAP_VALUE &&
		map_value_has_spin_lock(reg->map_ptr);
}