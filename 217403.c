static __always_inline void slab_unlock(struct page *page)
{
	VM_BUG_ON_PAGE(PageTail(page), page);
	__bit_spin_unlock(PG_locked, &page->flags);
}