static int __check_stack_boundary(struct bpf_verifier_env *env, u32 regno,
				  int off, int access_size,
				  bool zero_size_allowed)
{
	struct bpf_reg_state *reg = reg_state(env, regno);

	if (off >= 0 || off < -MAX_BPF_STACK || off + access_size > 0 ||
	    access_size < 0 || (access_size == 0 && !zero_size_allowed)) {
		if (tnum_is_const(reg->var_off)) {
			verbose(env, "invalid stack type R%d off=%d access_size=%d\n",
				regno, off, access_size);
		} else {
			char tn_buf[48];

			tnum_strn(tn_buf, sizeof(tn_buf), reg->var_off);
			verbose(env, "invalid stack type R%d var_off=%s access_size=%d\n",
				regno, tn_buf, access_size);
		}
		return -EACCES;
	}
	return 0;
}