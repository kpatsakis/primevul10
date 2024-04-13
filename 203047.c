void agp_free_page_array(struct agp_memory *mem)
{
	if (is_vmalloc_addr(mem->pages)) {
		vfree(mem->pages);
	} else {
		kfree(mem->pages);
	}
}