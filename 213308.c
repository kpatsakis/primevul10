static int mark_chain_precision_stack(struct bpf_verifier_env *env, int spi)
{
	return __mark_chain_precision(env, -1, spi);
}