BPF_CALL_3(bpf_get_stackid, struct pt_regs *, regs, struct bpf_map *, map,
	   u64, flags)
{
	u32 max_depth = map->value_size / stack_map_data_size(map);
	/* stack_map_alloc() checks that max_depth <= sysctl_perf_event_max_stack */
	u32 init_nr = sysctl_perf_event_max_stack - max_depth;
	bool user = flags & BPF_F_USER_STACK;
	struct perf_callchain_entry *trace;
	bool kernel = !user;

	if (unlikely(flags & ~(BPF_F_SKIP_FIELD_MASK | BPF_F_USER_STACK |
			       BPF_F_FAST_STACK_CMP | BPF_F_REUSE_STACKID)))
		return -EINVAL;

	trace = get_perf_callchain(regs, init_nr, kernel, user,
				   sysctl_perf_event_max_stack, false, false);

	if (unlikely(!trace))
		/* couldn't fetch the stack trace */
		return -EFAULT;

	return __bpf_get_stackid(map, trace, flags);
}