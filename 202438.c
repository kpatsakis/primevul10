void neigh_changeaddr(struct neigh_table *tbl, struct net_device *dev)
{
	int i;

	write_lock_bh(&tbl->lock);

	for (i=0; i <= tbl->hash_mask; i++) {
		struct neighbour *n, **np;

		np = &tbl->hash_buckets[i];
		while ((n = *np) != NULL) {
			if (dev && n->dev != dev) {
				np = &n->next;
				continue;
			}
			*np = n->next;
			write_lock_bh(&n->lock);
			n->dead = 1;
			neigh_del_timer(n);
			write_unlock_bh(&n->lock);
			neigh_release(n);
		}
	}

        write_unlock_bh(&tbl->lock);
}