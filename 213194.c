static void release_reg_references(struct bpf_verifier_env *env,
				   struct bpf_func_state *state,
				   int ref_obj_id)
{
	struct bpf_reg_state *regs = state->regs, *reg;
	int i;

	for (i = 0; i < MAX_BPF_REG; i++)
		if (regs[i].ref_obj_id == ref_obj_id)
			mark_reg_unknown(env, regs, i);

	bpf_for_each_spilled_reg(i, state, reg) {
		if (!reg)
			continue;
		if (reg->ref_obj_id == ref_obj_id)
			__mark_reg_unknown(env, reg);
	}
}