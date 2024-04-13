void agp_generic_destroy_page(struct page *page, int flags)
{
	if (page == NULL)
		return;

	if (flags & AGP_PAGE_DESTROY_UNMAP)
		unmap_page_from_agp(page);

	if (flags & AGP_PAGE_DESTROY_FREE) {
		put_page(page);
		__free_page(page);
		atomic_dec(&agp_bridge->current_memory_agp);
	}
}