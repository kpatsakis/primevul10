BPF_CALL_3(bpf_get_stackid_pe, struct bpf_perf_event_data_kern *, ctx,
	   struct bpf_map *, map, u64, flags)
{
	struct perf_event *event = ctx->event;
	struct perf_callchain_entry *trace;
	bool kernel, user;
	__u64 nr_kernel;
	int ret;

	/* perf_sample_data doesn't have callchain, use bpf_get_stackid */
	if (!(event->attr.sample_type & __PERF_SAMPLE_CALLCHAIN_EARLY))
		return bpf_get_stackid((unsigned long)(ctx->regs),
				       (unsigned long) map, flags, 0, 0);

	if (unlikely(flags & ~(BPF_F_SKIP_FIELD_MASK | BPF_F_USER_STACK |
			       BPF_F_FAST_STACK_CMP | BPF_F_REUSE_STACKID)))
		return -EINVAL;

	user = flags & BPF_F_USER_STACK;
	kernel = !user;

	trace = ctx->data->callchain;
	if (unlikely(!trace))
		return -EFAULT;

	nr_kernel = count_kernel_ip(trace);

	if (kernel) {
		__u64 nr = trace->nr;

		trace->nr = nr_kernel;
		ret = __bpf_get_stackid(map, trace, flags);

		/* restore nr */
		trace->nr = nr;
	} else { /* user */
		u64 skip = flags & BPF_F_SKIP_FIELD_MASK;

		skip += nr_kernel;
		if (skip > BPF_F_SKIP_FIELD_MASK)
			return -EFAULT;

		flags = (flags & ~BPF_F_SKIP_FIELD_MASK) | skip;
		ret = __bpf_get_stackid(map, trace, flags);
	}
	return ret;
}