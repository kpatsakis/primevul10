shmem_swapin(struct shmem_inode_info *info,swp_entry_t entry,unsigned long idx)
{
	swapin_readahead(entry, 0, NULL);
	return read_swap_cache_async(entry, NULL, 0);
}