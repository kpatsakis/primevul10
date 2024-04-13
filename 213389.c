static void __mark_ptr_or_null_regs(struct bpf_func_state *state, u32 id,
				    bool is_null)
{
	struct bpf_reg_state *reg;
	int i;

	for (i = 0; i < MAX_BPF_REG; i++)
		mark_ptr_or_null_reg(state, &state->regs[i], id, is_null);

	bpf_for_each_spilled_reg(i, state, reg) {
		if (!reg)
			continue;
		mark_ptr_or_null_reg(state, reg, id, is_null);
	}
}