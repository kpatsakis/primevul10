static FORCEINLINE void GetThreadCache(nedpool *RESTRICT *RESTRICT p, threadcache *RESTRICT *RESTRICT tc, int *RESTRICT mymspace, size_t *RESTRICT size) THROWSPEC
{
	int mycache;
#if THREADCACHEMAX
	if(size && *size<sizeof(threadcacheblk)) *size=sizeof(threadcacheblk);
#endif
	if(!*p)
		GetThreadCache_cold1(p);
	mycache=(int)(size_t) TLSGET((*p)->mycache);
	if(mycache>0)
	{	/* Already have a cache */
		*tc=(*p)->caches[mycache-1];
		*mymspace=(*tc)->mymspace;
	}
	else GetThreadCache_cold2(p, tc, mymspace, mycache);
	assert(*mymspace>=0);
#if USE_LOCKS
	assert(!(*tc) || (long)(size_t)CURRENT_THREAD==(*tc)->threadid);
#endif
#ifdef FULLSANITYCHECKS
	if(*tc)
	{
		if(*(unsigned int *)"NEDMALC1"!=(*tc)->magic1 || *(unsigned int *)"NEDMALC2"!=(*tc)->magic2)
		{
			abort();
		}
	}
#endif
}