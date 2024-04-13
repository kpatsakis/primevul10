static int stack_map_get_next_key(struct bpf_map *map, void *key,
				  void *next_key)
{
	struct bpf_stack_map *smap = container_of(map,
						  struct bpf_stack_map, map);
	u32 id;

	WARN_ON_ONCE(!rcu_read_lock_held());

	if (!key) {
		id = 0;
	} else {
		id = *(u32 *)key;
		if (id >= smap->n_buckets || !smap->buckets[id])
			id = 0;
		else
			id++;
	}

	while (id < smap->n_buckets && !smap->buckets[id])
		id++;

	if (id >= smap->n_buckets)
		return -ENOENT;

	*(u32 *)next_key = id;
	return 0;
}