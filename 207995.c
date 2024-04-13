NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR void * nedprealloc2(nedpool *p, void *mem, size_t size, size_t alignment, unsigned flags) THROWSPEC
{
	void *ret=0;
	threadcache *tc;
	int mymspace, isforeign=1;
	size_t memsize;
	if(!mem) return nedpmalloc2(p, size, alignment, flags);
#if REALLOC_ZERO_BYTES_FREES
	if(!size)
	{
		nedpfree2(p, mem, flags);
		return 0;
	}
#endif
	memsize=nedblksize(&isforeign, mem, flags);
	assert(memsize);
	if(!memsize)
	{
		fprintf(stderr, "nedmalloc: nedprealloc() called with a block not created by nedmalloc!\n");
		abort();
	}
	else if(size<=memsize && memsize-size<
#ifdef DEBUG
		32
#else
		1024
#endif
		)		/* If realloc size is within 1Kb smaller than existing, noop it */
		return mem;
	GetThreadCache(&p, &tc, &mymspace, &size);
#if THREADCACHEMAX
	if(alignment<=MALLOC_ALIGNMENT && !(flags & NM_FLAGS_MASK) && tc && size && size<=THREADCACHEMAX)
	{	/* Use the thread cache */
		if((ret=threadcache_malloc(p, tc, &size)))
		{
			size_t tocopy=memsize<size ? memsize : size;
			memcpy(ret, mem, tocopy);
			if((flags & M2_ZERO_MEMORY) && size>memsize)
				memset((void *)((size_t)ret+memsize), 0, size-memsize);
			LogOperation(tc, p, LOGENTRY_THREADCACHE_MALLOC, mymspace, size, mem, alignment, flags, ret);
			if(!isforeign && memsize>=sizeof(threadcacheblk) && memsize<=(THREADCACHEMAX+CHUNK_OVERHEAD))
			{
				threadcache_free(p, tc, mymspace, mem, memsize, isforeign);
				LogOperation(tc, p, LOGENTRY_THREADCACHE_FREE, mymspace, memsize, mem, 0, 0, 0);
			}
			else
			{
				CallFree(0, mem, isforeign);
				LogOperation(tc, p, LOGENTRY_POOL_FREE, mymspace, memsize, mem, 0, 0, 0);
			}
		}
	}
#endif
	if(!ret)
	{	/* Reallocs always happen in the mspace they happened in, so skip
		locking the preferred mspace for this thread */
		ret=CallRealloc(p->m[mymspace], mem, isforeign, memsize, size, alignment, flags);
		if(ret)
			LogOperation(tc, p, LOGENTRY_POOL_REALLOC, mymspace, size, mem, alignment, flags, ret);
	}
	LogOperation(tc, p, LOGENTRY_REALLOC, mymspace, size, mem, alignment, flags, ret);
	return ret;
}