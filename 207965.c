NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR void * nedpcalloc(nedpool *p, size_t no, size_t size) THROWSPEC
{
	size_t bytes=no*size;
	/* Avoid multiplication overflow. */
	if(size && no!=bytes/size)
		return 0;
	unsigned flags=NEDMALLOC_FORCERESERVE(p, 0, bytes);
	return nedpmalloc2(p, bytes, 0, M2_ZERO_MEMORY|flags);
}