void agp_alloc_page_array(size_t size, struct agp_memory *mem)
{
	mem->pages = NULL;

	if (size <= 2*PAGE_SIZE)
		mem->pages = kmalloc(size, GFP_KERNEL | __GFP_NOWARN);
	if (mem->pages == NULL) {
		mem->pages = vmalloc(size);
	}
}