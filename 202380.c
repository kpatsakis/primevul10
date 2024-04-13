static struct mfc_cache *ipmr_cache_alloc(void)
{
	struct mfc_cache *c=kmem_cache_alloc(mrt_cachep, GFP_KERNEL);
	if(c==NULL)
		return NULL;
	memset(c, 0, sizeof(*c));
	c->mfc_un.res.minvif = MAXVIFS;
	return c;
}