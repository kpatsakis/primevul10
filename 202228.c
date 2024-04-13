struct neigh_parms *neigh_parms_alloc(struct net_device *dev,
				      struct neigh_table *tbl)
{
	struct neigh_parms *p = kmalloc(sizeof(*p), GFP_KERNEL);

	if (p) {
		memcpy(p, &tbl->parms, sizeof(*p));
		p->tbl		  = tbl;
		atomic_set(&p->refcnt, 1);
		INIT_RCU_HEAD(&p->rcu_head);
		p->reachable_time =
				neigh_rand_reach_time(p->base_reachable_time);
		if (dev) {
			if (dev->neigh_setup && dev->neigh_setup(dev, p)) {
				kfree(p);
				return NULL;
			}

			dev_hold(dev);
			p->dev = dev;
		}
		p->sysctl_table = NULL;
		write_lock_bh(&tbl->lock);
		p->next		= tbl->parms.next;
		tbl->parms.next = p;
		write_unlock_bh(&tbl->lock);
	}
	return p;
}