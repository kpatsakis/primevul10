static int __check_packet_access(struct bpf_verifier_env *env, u32 regno,
				 int off, int size, bool zero_size_allowed)
{
	struct bpf_reg_state *regs = cur_regs(env);
	struct bpf_reg_state *reg = &regs[regno];

	if (off < 0 || size < 0 || (size == 0 && !zero_size_allowed) ||
	    (u64)off + size > reg->range) {
		verbose(env, "invalid access to packet, off=%d size=%d, R%d(id=%d,off=%d,r=%d)\n",
			off, size, regno, reg->id, reg->off, reg->range);
		return -EACCES;
	}
	return 0;
}