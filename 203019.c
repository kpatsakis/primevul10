struct agp_memory *agp_generic_alloc_user(size_t page_count, int type)
{
	struct agp_memory *new;
	int i;
	int pages;

	pages = (page_count + ENTRIES_PER_PAGE - 1) / ENTRIES_PER_PAGE;
	new = agp_create_user_memory(page_count);
	if (new == NULL)
		return NULL;

	for (i = 0; i < page_count; i++)
		new->pages[i] = NULL;
	new->page_count = 0;
	new->type = type;
	new->num_scratch_pages = pages;

	return new;
}