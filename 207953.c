NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR void * nedpmemalign(nedpool *p, size_t alignment, size_t bytes) THROWSPEC
{
	unsigned flags=NEDMALLOC_FORCERESERVE(p, 0, bytes);
	return nedpmalloc2(p, bytes, alignment, flags);
}