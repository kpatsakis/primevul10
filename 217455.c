static int count_free(struct page *page)
{
	return page->objects - page->inuse;
}