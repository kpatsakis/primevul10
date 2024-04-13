static FORCEINLINE void CallFree(void *RESTRICT mspace, void *RESTRICT mem, int isforeign) THROWSPEC
{
#if USE_MAGIC_HEADERS
	mstate oldmspace=0;
	size_t *_mem=(size_t *) mem-3, oldsize=0;
#endif
	if(isforeign)
	{
#if USE_MAGIC_HEADERS
		assert(_mem[0]!=*(size_t *) "NEDMALOC");
#endif
#if defined(DEBUG)
		printf("*** nedmalloc frees system allocated block %p\n", mem);
#endif
		sysfree(mem);
		return;
	}
#if USE_MAGIC_HEADERS
	assert(_mem[0]==*(size_t *) "NEDMALOC");
	oldmspace=(mstate) _mem[1];
	oldsize=_mem[2];
	for(; *_mem==*(size_t *) "NEDMALOC"; *_mem--=*(size_t *) "nedmaloc");
	mem=(void *)(++_mem);
#endif
#if USE_ALLOCATOR==0
	sysfree(mem);
#elif USE_ALLOCATOR==1
	mspace_free((mstate) mspace, mem);
#endif
}