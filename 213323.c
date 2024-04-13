static u32 state_htab_size(struct bpf_verifier_env *env)
{
	return env->prog->len;
}