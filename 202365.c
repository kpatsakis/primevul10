struct neighbour *neigh_lookup_nodev(struct neigh_table *tbl, const void *pkey)
{
	struct neighbour *n;
	int key_len = tbl->key_len;
	u32 hash_val = tbl->hash(pkey, NULL) & tbl->hash_mask;

	NEIGH_CACHE_STAT_INC(tbl, lookups);

	read_lock_bh(&tbl->lock);
	for (n = tbl->hash_buckets[hash_val]; n; n = n->next) {
		if (!memcmp(n->primary_key, pkey, key_len)) {
			neigh_hold(n);
			NEIGH_CACHE_STAT_INC(tbl, hits);
			break;
		}
	}
	read_unlock_bh(&tbl->lock);
	return n;
}