__printf(3, 4) static void verbose_linfo(struct bpf_verifier_env *env,
					 u32 insn_off,
					 const char *prefix_fmt, ...)
{
	const struct bpf_line_info *linfo;

	if (!bpf_verifier_log_needed(&env->log))
		return;

	linfo = find_linfo(env, insn_off);
	if (!linfo || linfo == env->prev_linfo)
		return;

	if (prefix_fmt) {
		va_list args;

		va_start(args, prefix_fmt);
		bpf_verifier_vlog(&env->log, prefix_fmt, args);
		va_end(args);
	}

	verbose(env, "%s\n",
		ltrim(btf_name_by_offset(env->prog->aux->btf,
					 linfo->line_off)));

	env->prev_linfo = linfo;
}