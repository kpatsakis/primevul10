static inline bool cmpxchg_double_slab(struct kmem_cache *s, struct page *page,
		void *freelist_old, unsigned long counters_old,
		void *freelist_new, unsigned long counters_new,
		const char *n)
{
#if defined(CONFIG_HAVE_CMPXCHG_DOUBLE) && \
    defined(CONFIG_HAVE_ALIGNED_STRUCT_PAGE)
	if (s->flags & __CMPXCHG_DOUBLE) {
		if (cmpxchg_double(&page->freelist, &page->counters,
				   freelist_old, counters_old,
				   freelist_new, counters_new))
			return true;
	} else
#endif
	{
		unsigned long flags;

		local_irq_save(flags);
		slab_lock(page);
		if (page->freelist == freelist_old &&
					page->counters == counters_old) {
			page->freelist = freelist_new;
			page->counters = counters_new;
			slab_unlock(page);
			local_irq_restore(flags);
			return true;
		}
		slab_unlock(page);
		local_irq_restore(flags);
	}

	cpu_relax();
	stat(s, CMPXCHG_DOUBLE_FAIL);

#ifdef SLUB_DEBUG_CMPXCHG
	pr_info("%s %s: cmpxchg double redo ", n, s->name);
#endif

	return false;
}