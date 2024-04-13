__clusterip_config_find(__be32 clusterip)
{
	struct clusterip_config *c;

	list_for_each_entry_rcu(c, &clusterip_configs, list) {
		if (c->clusterip == clusterip)
			return c;
	}

	return NULL;
}