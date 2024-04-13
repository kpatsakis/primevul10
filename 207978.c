static FORCEINLINE NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR void *CallRealloc(void *RESTRICT mspace, void *RESTRICT mem, int isforeign, size_t oldsize, size_t newsize, size_t alignment, unsigned flags) THROWSPEC
{
	void *RESTRICT ret=0;
#if USE_MAGIC_HEADERS
	mstate oldmspace=0;
	size_t *_ret=0, *_mem=(size_t *) mem-3;
#endif
	if(isforeign)
	{	/* Transfer */
#if USE_MAGIC_HEADERS
		assert(_mem[0]!=*(size_t *) "NEDMALOC");
#endif
		if((ret=CallMalloc(mspace, newsize, alignment, flags)))
		{
#if defined(DEBUG)
			printf("*** nedmalloc frees system allocated block %p\n", mem);
#endif
			memcpy(ret, mem, oldsize<newsize ? oldsize : newsize);
			sysfree(mem);
		}
		return ret;
	}
#if USE_MAGIC_HEADERS
	assert(_mem[0]==*(size_t *) "NEDMALOC");
	newsize+=3*sizeof(size_t);
	oldmspace=(mstate) _mem[1];
	assert(oldsize>=_mem[2]);
	for(; *_mem==*(size_t *) "NEDMALOC"; *_mem--=*(size_t *) "nedmaloc");
	mem=(void *)(++_mem);
#endif
#if USE_ALLOCATOR==0
	ret=sysrealloc(mem, newsize);
#elif USE_ALLOCATOR==1
	ret=mspace_realloc2((mstate) mspace, mem, newsize, alignment, flags);
#ifndef ENABLE_FAST_HEAP_DETECTION
	if(ret)
	{
		mchunkptr p=mem2chunk(ret);
		size_t truesize=chunksize(p) - overhead_for(p);
		if(!leastusedaddress || (void *)((mstate) mspace)->least_addr<leastusedaddress) leastusedaddress=(void *)((mstate) mspace)->least_addr;
		if(!largestusedblock || truesize>largestusedblock) largestusedblock=(truesize+mparams.page_size) & ~(mparams.page_size-1);
	}
#endif
#endif
	if(!ret)
	{	/* Put it back the way it was */
#if USE_MAGIC_HEADERS
		for(; *_mem==0; *_mem++=*(size_t *) "NEDMALOC");
#endif
		return 0;
	}
#if USE_MAGIC_HEADERS
	_ret=(size_t *) ret;
	ret=(void *)(_ret+3);
	for(; _ret<(size_t *)ret-2; _ret++) *_ret=*(size_t *) "NEDMALOC";
	_ret[0]=(size_t) mspace;
	_ret[1]=newsize-3*sizeof(size_t);
#endif
	return ret;
}