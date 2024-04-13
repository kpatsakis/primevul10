NEDMALLOCNOALIASATTR size_t nedblksize(int *RESTRICT isforeign, void *RESTRICT mem, unsigned flags) THROWSPEC
{
	if(mem)
	{
		if(isforeign) *isforeign=1;
#if USE_MAGIC_HEADERS
		{
			size_t *_mem=(size_t *) mem-3;
			if(_mem[0]==*(size_t *) "NEDMALOC")
			{
				mstate mspace=(mstate) _mem[1];
				size_t size=_mem[2];
				if(isforeign) *isforeign=0;
				return size;
			}
		}
#elif USE_ALLOCATOR==1
		if((flags & NM_SKIP_TOLERANCE_CHECKS) || nedblkmstate(mem))
		{
			mchunkptr p=mem2chunk(mem);
			if(isforeign) *isforeign=0;
			return chunksize(p)-overhead_for(p);
		}
#ifdef DEBUG
		else
		{
			int a=1; /* Set breakpoints here if needed */
		}
#endif
#endif
#if defined(ENABLE_TOLERANT_NEDMALLOC) || USE_ALLOCATOR==0
		return sysblksize(mem);
#endif
	}
	return 0;
}