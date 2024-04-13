NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR void **nedpindependent_calloc(nedpool *p, size_t elemsno, size_t elemsize, void **chunks) THROWSPEC
{
	void **ret;
	threadcache *tc;
	int mymspace;
	GetThreadCache(&p, &tc, &mymspace, &elemsize);
#if USE_ALLOCATOR==0
    GETMSPACE(m, p, tc, mymspace, elemsno*elemsize,
              ret=unsupported_operation("independent_calloc"));
#elif USE_ALLOCATOR==1
    GETMSPACE(m, p, tc, mymspace, elemsno*elemsize,
              ret=mspace_independent_calloc(m, elemsno, elemsize, chunks));
#endif
#if ENABLE_LOGGING
	if(ret && (ENABLE_LOGGING & LOGENTRY_POOL_MALLOC))
	{
		size_t n;
		for(n=0; n<elemsno; n++)
		{
			LogOperation(tc, p, LOGENTRY_POOL_MALLOC, mymspace, elemsize, 0, 0, M2_ZERO_MEMORY, ret[n]);
		}
	}
#endif
	return ret;
}