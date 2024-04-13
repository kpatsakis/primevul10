struct vm_area_struct *vm_area_dup(struct vm_area_struct *orig)
{
	struct vm_area_struct *new = kmem_cache_alloc(vm_area_cachep, GFP_KERNEL);

	if (new) {
		ASSERT_EXCLUSIVE_WRITER(orig->vm_flags);
		ASSERT_EXCLUSIVE_WRITER(orig->vm_file);
		/*
		 * orig->shared.rb may be modified concurrently, but the clone
		 * will be reinitialized.
		 */
		*new = data_race(*orig);
		INIT_LIST_HEAD(&new->anon_vma_chain);
		new->vm_next = new->vm_prev = NULL;
	}
	return new;
}