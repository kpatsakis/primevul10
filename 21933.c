static void tcf_block_offload_inc(struct tcf_block *block, u32 *flags)
{
	if (*flags & TCA_CLS_FLAGS_IN_HW)
		return;
	*flags |= TCA_CLS_FLAGS_IN_HW;
	atomic_inc(&block->offloadcnt);
}