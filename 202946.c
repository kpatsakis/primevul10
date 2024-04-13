clusterip_config_init(const struct ipt_clusterip_tgt_info *i, __be32 ip,
			struct net_device *dev)
{
	struct clusterip_config *c;

	c = kzalloc(sizeof(*c), GFP_ATOMIC);
	if (!c)
		return NULL;

	c->dev = dev;
	c->clusterip = ip;
	memcpy(&c->clustermac, &i->clustermac, ETH_ALEN);
	c->num_total_nodes = i->num_total_nodes;
	clusterip_config_init_nodelist(c, i);
	c->hash_mode = i->hash_mode;
	c->hash_initval = i->hash_initval;
	atomic_set(&c->refcount, 1);
	atomic_set(&c->entries, 1);

#ifdef CONFIG_PROC_FS
	{
		char buffer[16];

		/* create proc dir entry */
		sprintf(buffer, "%pI4", &ip);
		c->pde = proc_create_data(buffer, S_IWUSR|S_IRUSR,
					  clusterip_procdir,
					  &clusterip_proc_fops, c);
		if (!c->pde) {
			kfree(c);
			return NULL;
		}
	}
#endif

	spin_lock_bh(&clusterip_lock);
	list_add_rcu(&c->list, &clusterip_configs);
	spin_unlock_bh(&clusterip_lock);

	return c;
}