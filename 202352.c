static void neigh_periodic_timer(unsigned long arg)
{
	struct neigh_table *tbl = (struct neigh_table *)arg;
	struct neighbour *n, **np;
	unsigned long expire, now = jiffies;

	NEIGH_CACHE_STAT_INC(tbl, periodic_gc_runs);

	write_lock(&tbl->lock);

	/*
	 *	periodically recompute ReachableTime from random function
	 */

	if (time_after(now, tbl->last_rand + 300 * HZ)) {
		struct neigh_parms *p;
		tbl->last_rand = now;
		for (p = &tbl->parms; p; p = p->next)
			p->reachable_time =
				neigh_rand_reach_time(p->base_reachable_time);
	}

	np = &tbl->hash_buckets[tbl->hash_chain_gc];
	tbl->hash_chain_gc = ((tbl->hash_chain_gc + 1) & tbl->hash_mask);

	while ((n = *np) != NULL) {
		unsigned int state;

		write_lock(&n->lock);

		state = n->nud_state;
		if (state & (NUD_PERMANENT | NUD_IN_TIMER)) {
			write_unlock(&n->lock);
			goto next_elt;
		}

		if (time_before(n->used, n->confirmed))
			n->used = n->confirmed;

		if (atomic_read(&n->refcnt) == 1 &&
		    (state == NUD_FAILED ||
		     time_after(now, n->used + n->parms->gc_staletime))) {
			*np = n->next;
			n->dead = 1;
			write_unlock(&n->lock);
			neigh_release(n);
			continue;
		}
		write_unlock(&n->lock);

next_elt:
		np = &n->next;
	}

 	/* Cycle through all hash buckets every base_reachable_time/2 ticks.
 	 * ARP entry timeouts range from 1/2 base_reachable_time to 3/2
 	 * base_reachable_time.
	 */
	expire = tbl->parms.base_reachable_time >> 1;
	expire /= (tbl->hash_mask + 1);
	if (!expire)
		expire = 1;

 	mod_timer(&tbl->gc_timer, now + expire);

	write_unlock(&tbl->lock);
}