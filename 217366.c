static int list_locations(struct kmem_cache *s, char *buf,
					enum track_item alloc)
{
	int len = 0;
	unsigned long i;
	struct loc_track t = { 0, 0, NULL };
	int node;
	struct kmem_cache_node *n;

	if (!alloc_loc_track(&t, PAGE_SIZE / sizeof(struct location),
			     GFP_KERNEL)) {
		return sprintf(buf, "Out of memory\n");
	}
	/* Push back cpu slabs */
	flush_all(s);

	for_each_kmem_cache_node(s, node, n) {
		unsigned long flags;
		struct page *page;

		if (!atomic_long_read(&n->nr_slabs))
			continue;

		spin_lock_irqsave(&n->list_lock, flags);
		list_for_each_entry(page, &n->partial, slab_list)
			process_slab(&t, s, page, alloc);
		list_for_each_entry(page, &n->full, slab_list)
			process_slab(&t, s, page, alloc);
		spin_unlock_irqrestore(&n->list_lock, flags);
	}

	for (i = 0; i < t.count; i++) {
		struct location *l = &t.loc[i];

		if (len > PAGE_SIZE - KSYM_SYMBOL_LEN - 100)
			break;
		len += sprintf(buf + len, "%7ld ", l->count);

		if (l->addr)
			len += sprintf(buf + len, "%pS", (void *)l->addr);
		else
			len += sprintf(buf + len, "<not-available>");

		if (l->sum_time != l->min_time) {
			len += sprintf(buf + len, " age=%ld/%ld/%ld",
				l->min_time,
				(long)div_u64(l->sum_time, l->count),
				l->max_time);
		} else
			len += sprintf(buf + len, " age=%ld",
				l->min_time);

		if (l->min_pid != l->max_pid)
			len += sprintf(buf + len, " pid=%ld-%ld",
				l->min_pid, l->max_pid);
		else
			len += sprintf(buf + len, " pid=%ld",
				l->min_pid);

		if (num_online_cpus() > 1 &&
				!cpumask_empty(to_cpumask(l->cpus)) &&
				len < PAGE_SIZE - 60)
			len += scnprintf(buf + len, PAGE_SIZE - len - 50,
					 " cpus=%*pbl",
					 cpumask_pr_args(to_cpumask(l->cpus)));

		if (nr_online_nodes > 1 && !nodes_empty(l->nodes) &&
				len < PAGE_SIZE - 60)
			len += scnprintf(buf + len, PAGE_SIZE - len - 50,
					 " nodes=%*pbl",
					 nodemask_pr_args(&l->nodes));

		len += sprintf(buf + len, "\n");
	}

	free_loc_track(&t);
	if (!t.count)
		len += sprintf(buf, "No data\n");
	return len;
}