int    nedpmalloc_trim(nedpool *p, size_t pad) THROWSPEC
{
	int n, ret=0;
	if(!p) { p=&syspool; if(!syspool.threads) InitPool(&syspool, 0, -1); }
	for(n=0; p->m[n]; n++)
	{
#if USE_ALLOCATOR==1
		ret+=mspace_trim(p->m[n], pad);
#endif
	}
	return ret;
}