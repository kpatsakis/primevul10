clusterip_config_find_get(__be32 clusterip, int entry)
{
	struct clusterip_config *c;

	rcu_read_lock_bh();
	c = __clusterip_config_find(clusterip);
	if (c) {
		if (unlikely(!atomic_inc_not_zero(&c->refcount)))
			c = NULL;
		else if (entry)
			atomic_inc(&c->entries);
	}
	rcu_read_unlock_bh();

	return c;
}