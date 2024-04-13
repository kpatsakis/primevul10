void neddestroypool(nedpool *p) THROWSPEC
{
	unsigned int n;
#if USE_LOCKS
	ACQUIRE_LOCK(&p->mutex);
#endif
	DestroyCaches(p);
	for(n=0; p->m[n]; n++)
	{
#if USE_ALLOCATOR==1
		destroy_mspace(p->m[n]);
#endif
		p->m[n]=0;
	}
#if USE_LOCKS
	RELEASE_LOCK(&p->mutex);
	DESTROY_LOCK(&p->mutex);
#endif
	if(TLSFREE(p->mycache)) abort();
	nedpfree(0, p);
#if USE_LOCKS
	ACQUIRE_LOCK(&poollistlock);
#endif
	assert(poollist);
	for(n=0; n<poollist->length && poollist->list[n]!=p; n++);
	assert(n!=poollist->length);
	memmove(&poollist->list[n], &poollist->list[n+1], (size_t)&poollist->list[poollist->length]-(size_t)&poollist->list[n]);
	if(!--poollist->length)
	{
		assert(!poollist->list[0]);
		nedpfree(0, poollist);
		poollist=0;
	}
#if USE_LOCKS
	RELEASE_LOCK(&poollistlock);
#endif
}