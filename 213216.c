static bool insn_has_def32(struct bpf_verifier_env *env, struct bpf_insn *insn)
{
	if (insn_no_def(insn))
		return false;

	return !is_reg64(env, insn, insn->dst_reg, NULL, DST_OP);
}