static NOINLINE int InitPool(nedpool *RESTRICT p, size_t capacity, int threads) THROWSPEC
{	/* threads is -1 for system pool */
	ensure_initialization();
	ACQUIRE_MALLOC_GLOBAL_LOCK();
	if(p->threads) goto done;
#if USE_LOCKS
	if(INITIAL_LOCK(&p->mutex)) goto err;
#endif
	if(TLSALLOC(&p->mycache)) goto err;
#if USE_ALLOCATOR==0
	p->m[0]=(mstate) mspacecounter++;
#elif USE_ALLOCATOR==1
	if(!(p->m[0]=(mstate) create_mspace(capacity, 1))) goto err;
	p->m[0]->extp=p;
#endif
	p->threads=(threads>MAXTHREADSINPOOL) ? MAXTHREADSINPOOL : (!threads) ? DEFAULTMAXTHREADSINPOOL : threads;
done:
	RELEASE_MALLOC_GLOBAL_LOCK();
	return 1;
err:
	if(threads<0)
		abort();			/* If you can't allocate for system pool, we're screwed */
	DestroyCaches(p);
	if(p->m[0])
	{
#if USE_ALLOCATOR==1
		destroy_mspace(p->m[0]);
#endif
		p->m[0]=0;
	}
	if(p->mycache)
	{
		if(TLSFREE(p->mycache)) abort();
		p->mycache=0;
	}
	RELEASE_MALLOC_GLOBAL_LOCK();
	return 0;
}