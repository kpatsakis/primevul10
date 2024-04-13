static struct page *shmem_swapin_async(struct shared_policy *p,
				       swp_entry_t entry, unsigned long idx)
{
	struct page *page;
	struct vm_area_struct pvma;

	/* Create a pseudo vma that just contains the policy */
	memset(&pvma, 0, sizeof(struct vm_area_struct));
	pvma.vm_end = PAGE_SIZE;
	pvma.vm_pgoff = idx;
	pvma.vm_policy = mpol_shared_policy_lookup(p, idx);
	page = read_swap_cache_async(entry, &pvma, 0);
	mpol_free(pvma.vm_policy);
	return page;
}