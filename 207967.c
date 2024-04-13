NEDMALLOCNOALIASATTR void*  nedmalloc_internals(size_t *granularity, size_t *magic) THROWSPEC
{
#if USE_ALLOCATOR==1
	if(granularity) *granularity=mparams.granularity;
	if(magic) *magic=mparams.magic;
	return (void *) &syspool;
#else
	if(granularity) *granularity=0;
	if(magic) *magic=0;
	return 0;
#endif
}