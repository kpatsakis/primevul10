static long __bpf_get_stackid(struct bpf_map *map,
			      struct perf_callchain_entry *trace, u64 flags)
{
	struct bpf_stack_map *smap = container_of(map, struct bpf_stack_map, map);
	struct stack_map_bucket *bucket, *new_bucket, *old_bucket;
	u32 max_depth = map->value_size / stack_map_data_size(map);
	/* stack_map_alloc() checks that max_depth <= sysctl_perf_event_max_stack */
	u32 init_nr = sysctl_perf_event_max_stack - max_depth;
	u32 skip = flags & BPF_F_SKIP_FIELD_MASK;
	u32 hash, id, trace_nr, trace_len;
	bool user = flags & BPF_F_USER_STACK;
	u64 *ips;
	bool hash_matches;

	/* get_perf_callchain() guarantees that trace->nr >= init_nr
	 * and trace-nr <= sysctl_perf_event_max_stack, so trace_nr <= max_depth
	 */
	trace_nr = trace->nr - init_nr;

	if (trace_nr <= skip)
		/* skipping more than usable stack trace */
		return -EFAULT;

	trace_nr -= skip;
	trace_len = trace_nr * sizeof(u64);
	ips = trace->ip + skip + init_nr;
	hash = jhash2((u32 *)ips, trace_len / sizeof(u32), 0);
	id = hash & (smap->n_buckets - 1);
	bucket = READ_ONCE(smap->buckets[id]);

	hash_matches = bucket && bucket->hash == hash;
	/* fast cmp */
	if (hash_matches && flags & BPF_F_FAST_STACK_CMP)
		return id;

	if (stack_map_use_build_id(map)) {
		/* for build_id+offset, pop a bucket before slow cmp */
		new_bucket = (struct stack_map_bucket *)
			pcpu_freelist_pop(&smap->freelist);
		if (unlikely(!new_bucket))
			return -ENOMEM;
		new_bucket->nr = trace_nr;
		stack_map_get_build_id_offset(
			(struct bpf_stack_build_id *)new_bucket->data,
			ips, trace_nr, user);
		trace_len = trace_nr * sizeof(struct bpf_stack_build_id);
		if (hash_matches && bucket->nr == trace_nr &&
		    memcmp(bucket->data, new_bucket->data, trace_len) == 0) {
			pcpu_freelist_push(&smap->freelist, &new_bucket->fnode);
			return id;
		}
		if (bucket && !(flags & BPF_F_REUSE_STACKID)) {
			pcpu_freelist_push(&smap->freelist, &new_bucket->fnode);
			return -EEXIST;
		}
	} else {
		if (hash_matches && bucket->nr == trace_nr &&
		    memcmp(bucket->data, ips, trace_len) == 0)
			return id;
		if (bucket && !(flags & BPF_F_REUSE_STACKID))
			return -EEXIST;

		new_bucket = (struct stack_map_bucket *)
			pcpu_freelist_pop(&smap->freelist);
		if (unlikely(!new_bucket))
			return -ENOMEM;
		memcpy(new_bucket->data, ips, trace_len);
	}

	new_bucket->hash = hash;
	new_bucket->nr = trace_nr;

	old_bucket = xchg(&smap->buckets[id], new_bucket);
	if (old_bucket)
		pcpu_freelist_push(&smap->freelist, &old_bucket->fnode);
	return id;
}