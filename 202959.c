clusterip_config_put(struct clusterip_config *c)
{
	if (atomic_dec_and_test(&c->refcount))
		call_rcu_bh(&c->rcu, clusterip_config_rcu_free);
}