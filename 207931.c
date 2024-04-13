static NOINLINE threadcache *AllocCache(nedpool *RESTRICT p) THROWSPEC
{
	threadcache *tc=0;
	int n, end;
#if USE_LOCKS
	ACQUIRE_LOCK(&p->mutex);
#endif
	for(n=0; n<THREADCACHEMAXCACHES && p->caches[n]; n++);
	if(THREADCACHEMAXCACHES==n)
	{	/* List exhausted, so disable for this thread */
#if USE_LOCKS
		RELEASE_LOCK(&p->mutex);
#endif
		return 0;
	}
	tc=p->caches[n]=(threadcache *) CallMalloc(p->m[0], sizeof(threadcache), 0, M2_ZERO_MEMORY);
	if(!tc)
	{
#if USE_LOCKS
		RELEASE_LOCK(&p->mutex);
#endif
		return 0;
	}
#ifdef FULLSANITYCHECKS
	tc->magic1=*(unsigned int *)"NEDMALC1";
	tc->magic2=*(unsigned int *)"NEDMALC2";
#endif
	tc->threadid=
#if USE_LOCKS
		(long)(size_t)CURRENT_THREAD;
#else
		1;
#endif
	for(end=0; p->m[end]; end++);
	tc->mymspace=abs(tc->threadid) % end;
#if ENABLE_LOGGING
	{
		mchunkptr cp;
		size_t logentrieslen=2048/sizeof(logentry);		/* One page */
		tc->logentries=tc->logentriesptr=(logentry *) CallMalloc(p->m[0], logentrieslen*sizeof(logentry), 0, M2_ZERO_MEMORY|M2_ALWAYS_MMAP|M2_RESERVE_MULT(8));
		if(!tc->logentries)
		{
#if USE_LOCKS
			RELEASE_LOCK(&p->mutex);
#endif
			return 0;
		}
		cp=mem2chunk(tc->logentries);
		logentrieslen=(chunksize(cp)-overhead_for(cp))/sizeof(logentry);
		tc->logentriesend=tc->logentries+logentrieslen;
	}
#endif
#if USE_LOCKS
	RELEASE_LOCK(&p->mutex);
#endif
	if(TLSSET(p->mycache, (void *)(size_t)(n+1))) abort();
	return tc;
}