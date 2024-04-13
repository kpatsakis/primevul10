void neigh_table_init(struct neigh_table *tbl)
{
	unsigned long now = jiffies;
	unsigned long phsize;

	atomic_set(&tbl->parms.refcnt, 1);
	INIT_RCU_HEAD(&tbl->parms.rcu_head);
	tbl->parms.reachable_time =
			  neigh_rand_reach_time(tbl->parms.base_reachable_time);

	if (!tbl->kmem_cachep)
		tbl->kmem_cachep = kmem_cache_create(tbl->id,
						     tbl->entry_size,
						     0, SLAB_HWCACHE_ALIGN,
						     NULL, NULL);

	if (!tbl->kmem_cachep)
		panic("cannot create neighbour cache");

	tbl->stats = alloc_percpu(struct neigh_statistics);
	if (!tbl->stats)
		panic("cannot create neighbour cache statistics");
	
#ifdef CONFIG_PROC_FS
	tbl->pde = create_proc_entry(tbl->id, 0, proc_net_stat);
	if (!tbl->pde) 
		panic("cannot create neighbour proc dir entry");
	tbl->pde->proc_fops = &neigh_stat_seq_fops;
	tbl->pde->data = tbl;
#endif

	tbl->hash_mask = 1;
	tbl->hash_buckets = neigh_hash_alloc(tbl->hash_mask + 1);

	phsize = (PNEIGH_HASHMASK + 1) * sizeof(struct pneigh_entry *);
	tbl->phash_buckets = kmalloc(phsize, GFP_KERNEL);

	if (!tbl->hash_buckets || !tbl->phash_buckets)
		panic("cannot allocate neighbour cache hashes");

	memset(tbl->phash_buckets, 0, phsize);

	get_random_bytes(&tbl->hash_rnd, sizeof(tbl->hash_rnd));

	rwlock_init(&tbl->lock);
	init_timer(&tbl->gc_timer);
	tbl->gc_timer.data     = (unsigned long)tbl;
	tbl->gc_timer.function = neigh_periodic_timer;
	tbl->gc_timer.expires  = now + 1;
	add_timer(&tbl->gc_timer);

	init_timer(&tbl->proxy_timer);
	tbl->proxy_timer.data	  = (unsigned long)tbl;
	tbl->proxy_timer.function = neigh_proxy_process;
	skb_queue_head_init(&tbl->proxy_queue);

	tbl->last_flush = now;
	tbl->last_rand	= now + tbl->parms.reachable_time * 20;
	write_lock(&neigh_tbl_lock);
	tbl->next	= neigh_tables;
	neigh_tables	= tbl;
	write_unlock(&neigh_tbl_lock);
}