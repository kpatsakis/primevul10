static int check_attach_modify_return(struct bpf_verifier_env *env)
{
	struct bpf_prog *prog = env->prog;
	unsigned long addr = (unsigned long) prog->aux->trampoline->func.addr;

	/* This is expected to be cleaned up in the future with the KRSI effort
	 * introducing the LSM_HOOK macro for cleaning up lsm_hooks.h.
	 */
	if (within_error_injection_list(addr) ||
	    !strncmp(SECURITY_PREFIX, prog->aux->attach_func_name,
		     sizeof(SECURITY_PREFIX) - 1))
		return 0;

	verbose(env, "fmod_ret attach_btf_id %u (%s) is not modifiable\n",
		prog->aux->attach_btf_id, prog->aux->attach_func_name);

	return -EINVAL;
}