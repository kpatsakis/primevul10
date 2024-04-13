BPF_CALL_4(bpf_get_stack_pe, struct bpf_perf_event_data_kern *, ctx,
	   void *, buf, u32, size, u64, flags)
{
	struct pt_regs *regs = (struct pt_regs *)(ctx->regs);
	struct perf_event *event = ctx->event;
	struct perf_callchain_entry *trace;
	bool kernel, user;
	int err = -EINVAL;
	__u64 nr_kernel;

	if (!(event->attr.sample_type & __PERF_SAMPLE_CALLCHAIN_EARLY))
		return __bpf_get_stack(regs, NULL, NULL, buf, size, flags);

	if (unlikely(flags & ~(BPF_F_SKIP_FIELD_MASK | BPF_F_USER_STACK |
			       BPF_F_USER_BUILD_ID)))
		goto clear;

	user = flags & BPF_F_USER_STACK;
	kernel = !user;

	err = -EFAULT;
	trace = ctx->data->callchain;
	if (unlikely(!trace))
		goto clear;

	nr_kernel = count_kernel_ip(trace);

	if (kernel) {
		__u64 nr = trace->nr;

		trace->nr = nr_kernel;
		err = __bpf_get_stack(regs, NULL, trace, buf, size, flags);

		/* restore nr */
		trace->nr = nr;
	} else { /* user */
		u64 skip = flags & BPF_F_SKIP_FIELD_MASK;

		skip += nr_kernel;
		if (skip > BPF_F_SKIP_FIELD_MASK)
			goto clear;

		flags = (flags & ~BPF_F_SKIP_FIELD_MASK) | skip;
		err = __bpf_get_stack(regs, NULL, trace, buf, size, flags);
	}
	return err;

clear:
	memset(buf, 0, size);
	return err;

}