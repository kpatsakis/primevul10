static long __bpf_get_stack(struct pt_regs *regs, struct task_struct *task,
			    struct perf_callchain_entry *trace_in,
			    void *buf, u32 size, u64 flags)
{
	u32 init_nr, trace_nr, copy_len, elem_size, num_elem;
	bool user_build_id = flags & BPF_F_USER_BUILD_ID;
	u32 skip = flags & BPF_F_SKIP_FIELD_MASK;
	bool user = flags & BPF_F_USER_STACK;
	struct perf_callchain_entry *trace;
	bool kernel = !user;
	int err = -EINVAL;
	u64 *ips;

	if (unlikely(flags & ~(BPF_F_SKIP_FIELD_MASK | BPF_F_USER_STACK |
			       BPF_F_USER_BUILD_ID)))
		goto clear;
	if (kernel && user_build_id)
		goto clear;

	elem_size = (user && user_build_id) ? sizeof(struct bpf_stack_build_id)
					    : sizeof(u64);
	if (unlikely(size % elem_size))
		goto clear;

	/* cannot get valid user stack for task without user_mode regs */
	if (task && user && !user_mode(regs))
		goto err_fault;

	num_elem = size / elem_size;
	if (sysctl_perf_event_max_stack < num_elem)
		init_nr = 0;
	else
		init_nr = sysctl_perf_event_max_stack - num_elem;

	if (trace_in)
		trace = trace_in;
	else if (kernel && task)
		trace = get_callchain_entry_for_task(task, init_nr);
	else
		trace = get_perf_callchain(regs, init_nr, kernel, user,
					   sysctl_perf_event_max_stack,
					   false, false);
	if (unlikely(!trace))
		goto err_fault;

	trace_nr = trace->nr - init_nr;
	if (trace_nr < skip)
		goto err_fault;

	trace_nr -= skip;
	trace_nr = (trace_nr <= num_elem) ? trace_nr : num_elem;
	copy_len = trace_nr * elem_size;
	ips = trace->ip + skip + init_nr;
	if (user && user_build_id)
		stack_map_get_build_id_offset(buf, ips, trace_nr, user);
	else
		memcpy(buf, ips, copy_len);

	if (size > copy_len)
		memset(buf + copy_len, 0, size - copy_len);
	return copy_len;

err_fault:
	err = -EFAULT;
clear:
	memset(buf, 0, size);
	return err;
}