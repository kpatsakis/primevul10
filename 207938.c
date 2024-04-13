static NOINLINE void ReleaseFreeInCache(nedpool *RESTRICT p, threadcache *RESTRICT tc, int mymspace) THROWSPEC
{
	unsigned int age=THREADCACHEMAXFREESPACE/8192;
#if USE_LOCKS
	/*ACQUIRE_LOCK(&p->m[mymspace]->mutex);*/
#endif
	while(age && tc->freeInCache>=THREADCACHEMAXFREESPACE)
	{
		RemoveCacheEntries(p, tc, age);
		/*printf("*** Removing cache entries older than %u (%u)\n", age, (unsigned int) tc->freeInCache);*/
		age>>=1;
	}
#if USE_LOCKS
	/*RELEASE_LOCK(&p->m[mymspace]->mutex);*/
#endif
}