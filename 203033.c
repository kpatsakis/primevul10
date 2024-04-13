void agp_free_memory(struct agp_memory *curr)
{
	size_t i;

	if (curr == NULL)
		return;

	if (curr->is_bound)
		agp_unbind_memory(curr);

	if (curr->type >= AGP_USER_TYPES) {
		agp_generic_free_by_type(curr);
		return;
	}

	if (curr->type != 0) {
		curr->bridge->driver->free_by_type(curr);
		return;
	}
	if (curr->page_count != 0) {
		if (curr->bridge->driver->agp_destroy_pages) {
			curr->bridge->driver->agp_destroy_pages(curr);
		} else {

			for (i = 0; i < curr->page_count; i++) {
				curr->bridge->driver->agp_destroy_page(
					curr->pages[i],
					AGP_PAGE_DESTROY_UNMAP);
			}
			for (i = 0; i < curr->page_count; i++) {
				curr->bridge->driver->agp_destroy_page(
					curr->pages[i],
					AGP_PAGE_DESTROY_FREE);
			}
		}
	}
	agp_free_key(curr->key);
	agp_free_page_array(curr);
	kfree(curr);
}