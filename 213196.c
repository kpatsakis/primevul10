static void sanitize_insn_aux_data(struct bpf_verifier_env *env)
{
	struct bpf_insn *insn = env->prog->insnsi;
	struct bpf_insn_aux_data *aux;
	int i, class;

	for (i = 0; i < env->prog->len; i++) {
		class = BPF_CLASS(insn[i].code);
		if (class != BPF_LDX && class != BPF_STX)
			continue;
		aux = &env->insn_aux_data[i];
		if (aux->seen != env->pass_cnt)
			continue;
		memset(aux, 0, offsetof(typeof(*aux), orig_idx));
	}
}