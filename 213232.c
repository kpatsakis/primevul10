static void __clear_all_pkt_pointers(struct bpf_verifier_env *env,
				     struct bpf_func_state *state)
{
	struct bpf_reg_state *regs = state->regs, *reg;
	int i;

	for (i = 0; i < MAX_BPF_REG; i++)
		if (reg_is_pkt_pointer_any(&regs[i]))
			mark_reg_unknown(env, regs, i);

	bpf_for_each_spilled_reg(i, state, reg) {
		if (!reg)
			continue;
		if (reg_is_pkt_pointer_any(reg))
			__mark_reg_unknown(env, reg);
	}
}