void nedtrimthreadcache(nedpool *p, int disable) THROWSPEC
{
	int mycache;
	if(!p)
	{
		p=&syspool;
		if(!syspool.threads) InitPool(&syspool, 0, -1);
	}
	mycache=(int)(size_t) TLSGET(p->mycache);
	if(!mycache)
	{	/* Set to mspace 0 */
		if(disable && TLSSET(p->mycache, (void *)(size_t)-1)) abort();
	}
	else if(mycache>0)
	{	/* Set to last used mspace */
		threadcache *tc=p->caches[mycache-1];
#if defined(DEBUG)
		printf("Threadcache utilisation: %lf%% in cache with %lf%% lost to other threads\n",
			100.0*tc->successes/tc->mallocs, 100.0*((double) tc->mallocs-tc->frees)/tc->mallocs);
#endif
		if(disable && TLSSET(p->mycache, (void *)(size_t)(-tc->mymspace))) abort();
		tc->frees++;
		RemoveCacheEntries(p, tc, 0);
		assert(!tc->freeInCache);
		if(disable)
		{
			tc->mymspace=-1;
			tc->threadid=0;
			CallFree(0, p->caches[mycache-1], 0);
			p->caches[mycache-1]=0;
		}
	}
}