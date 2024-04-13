static int count_inuse(struct page *page)
{
	return page->inuse;
}