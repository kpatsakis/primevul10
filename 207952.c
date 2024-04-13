NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR void * nedpmalloc2(nedpool *p, size_t size, size_t alignment, unsigned flags) THROWSPEC
{
	void *ret=0;
	threadcache *tc;
	int mymspace;
	GetThreadCache(&p, &tc, &mymspace, &size);
#if THREADCACHEMAX
	if(alignment<=MALLOC_ALIGNMENT && !(flags & NM_FLAGS_MASK) && tc && size<=THREADCACHEMAX)
	{	/* Use the thread cache */
		if((ret=threadcache_malloc(p, tc, &size)))
		{
			if((flags & M2_ZERO_MEMORY))
				memset(ret, 0, size);
			LogOperation(tc, p, LOGENTRY_THREADCACHE_MALLOC, mymspace, size, 0, alignment, flags, ret);
		}
	}
#endif
	if(!ret)
	{	/* Use this thread's mspace */
        GETMSPACE(m, p, tc, mymspace, size,
                  ret=CallMalloc(m, size, alignment, flags));
		if(ret)
			LogOperation(tc, p, LOGENTRY_POOL_MALLOC, mymspace, size, 0, alignment, flags, ret);
	}
	LogOperation(tc, p, LOGENTRY_MALLOC, mymspace, size, 0, alignment, flags, ret);
	return ret;
}