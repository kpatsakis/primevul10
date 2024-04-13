static bool tcf_block_offload_in_use(struct tcf_block *block)
{
	return atomic_read(&block->offloadcnt);
}