static void set_track(struct kmem_cache *s, void *object,
			enum track_item alloc, unsigned long addr)
{
	struct track *p = get_track(s, object, alloc);

	if (addr) {
#ifdef CONFIG_STACKTRACE
		unsigned int nr_entries;

		metadata_access_enable();
		nr_entries = stack_trace_save(p->addrs, TRACK_ADDRS_COUNT, 3);
		metadata_access_disable();

		if (nr_entries < TRACK_ADDRS_COUNT)
			p->addrs[nr_entries] = 0;
#endif
		p->addr = addr;
		p->cpu = smp_processor_id();
		p->pid = current->pid;
		p->when = jiffies;
	} else {
		memset(p, 0, sizeof(struct track));
	}
}