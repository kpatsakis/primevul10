static struct page *shmem_swapin(struct shmem_inode_info *info,
				 swp_entry_t entry, unsigned long idx)
{
	struct shared_policy *p = &info->policy;
	int i, num;
	struct page *page;
	unsigned long offset;

	num = valid_swaphandles(entry, &offset);
	for (i = 0; i < num; offset++, i++) {
		page = shmem_swapin_async(p,
				swp_entry(swp_type(entry), offset), idx);
		if (!page)
			break;
		page_cache_release(page);
	}
	lru_add_drain();	/* Push any new pages onto the LRU now */
	return shmem_swapin_async(p, entry, idx);
}