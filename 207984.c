NEDMALLOCNOALIASATTR void   nedpfree2(nedpool *p, void *mem, unsigned flags) THROWSPEC
{	/* Frees always happen in the mspace they happened in, so skip
	locking the preferred mspace for this thread */
	threadcache *tc;
	int mymspace, isforeign=1;
	size_t memsize;
	if(!mem)
	{	/* If you tried this on FreeBSD you'd be sorry! */
#ifdef DEBUG
		fprintf(stderr, "nedmalloc: WARNING nedpfree() called with zero. This is not portable behaviour!\n");
#endif
		return;
	}
	memsize=nedblksize(&isforeign, mem, flags);
	assert(memsize);
	if(!memsize)
	{
		fprintf(stderr, "nedmalloc: nedpfree() called with a block not created by nedmalloc!\n");
		abort();
	}
	GetThreadCache(&p, &tc, &mymspace, 0);
#if THREADCACHEMAX
	if(mem && tc && !isforeign && memsize>=sizeof(threadcacheblk) && memsize<=(THREADCACHEMAX+CHUNK_OVERHEAD))
	{
		threadcache_free(p, tc, mymspace, mem, memsize, isforeign);
		LogOperation(tc, p, LOGENTRY_THREADCACHE_FREE, mymspace, memsize, mem, 0, 0, 0);
	}
	else
#endif
	{
		CallFree(0, mem, isforeign);
		LogOperation(tc, p, LOGENTRY_POOL_FREE, mymspace, memsize, mem, 0, 0, 0);
	}
	LogOperation(tc, p, LOGENTRY_FREE, mymspace, memsize, mem, 0, 0, 0);
}