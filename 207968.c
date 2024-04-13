size_t nedpmalloc_footprint(nedpool *p) THROWSPEC
{
	size_t ret=0;
	int n;
	if(!p) { p=&syspool; if(!syspool.threads) InitPool(&syspool, 0, -1); }
	for(n=0; p->m[n]; n++)
	{
#if USE_ALLOCATOR==1
		ret+=mspace_footprint(p->m[n]);
#endif
	}
	return ret;
}