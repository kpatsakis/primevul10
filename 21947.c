BPF_CALL_4(bpf_get_task_stack, struct task_struct *, task, void *, buf,
	   u32, size, u64, flags)
{
	struct pt_regs *regs;
	long res;

	if (!try_get_task_stack(task))
		return -EFAULT;

	regs = task_pt_regs(task);
	res = __bpf_get_stack(regs, task, NULL, buf, size, flags);
	put_task_stack(task);

	return res;
}