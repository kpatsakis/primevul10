BPF_CALL_4(bpf_get_stack, struct pt_regs *, regs, void *, buf, u32, size,
	   u64, flags)
{
	return __bpf_get_stack(regs, NULL, NULL, buf, size, flags);
}