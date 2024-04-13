static bool insn_no_def(struct bpf_insn *insn)
{
	u8 class = BPF_CLASS(insn->code);

	return (class == BPF_JMP || class == BPF_JMP32 ||
		class == BPF_STX || class == BPF_ST);
}