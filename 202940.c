clusterip_config_get(struct clusterip_config *c)
{
	atomic_inc(&c->refcount);
}