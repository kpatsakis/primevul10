static NOINLINE void GetThreadCache_cold2(nedpool *RESTRICT *RESTRICT p, threadcache *RESTRICT *RESTRICT tc, int *RESTRICT mymspace, int mycache) THROWSPEC
{
	if(!mycache)
	{	/* Need to allocate a new cache */
		*tc=AllocCache(*p);
		if(!*tc)
		{	/* Disable */
			if(TLSSET((*p)->mycache, (void *)(size_t)-1)) abort();
			*mymspace=0;
		}
		else
			*mymspace=(*tc)->mymspace;
	}
	else
	{	/* Cache disabled, but we do have an assigned thread pool */
		*tc=0;
		*mymspace=-mycache-1;
	}
}