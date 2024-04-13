NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR void **nedpindependent_comalloc(nedpool *p, size_t elems, size_t *sizes, void **chunks) THROWSPEC
{
	void **ret;
	threadcache *tc;
	int mymspace;
    size_t i, *adjustedsizes=(size_t *) alloca(elems*sizeof(size_t));
    if(!adjustedsizes) return 0;
    for(i=0; i<elems; i++)
        adjustedsizes[i]=sizes[i]<sizeof(threadcacheblk) ? sizeof(threadcacheblk) : sizes[i];
	GetThreadCache(&p, &tc, &mymspace, 0);
#if USE_ALLOCATOR==0
	GETMSPACE(m, p, tc, mymspace, 0,
              ret=unsupported_operation("independent_comalloc"));
#elif USE_ALLOCATOR==1
	GETMSPACE(m, p, tc, mymspace, 0,
              ret=mspace_independent_comalloc(m, elems, adjustedsizes, chunks));
#endif
#if ENABLE_LOGGING
	if(ret && (ENABLE_LOGGING & LOGENTRY_POOL_MALLOC))
	{
		size_t n;
		for(n=0; n<elems; n++)
		{
			LogOperation(tc, p, LOGENTRY_POOL_MALLOC, mymspace, sizes[n], 0, 0, 0, ret[n]);
		}
	}
#endif
	return ret;
}