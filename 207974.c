NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR void * nedmalloc2(size_t size, size_t alignment, unsigned flags) THROWSPEC				{ return nedpmalloc2((nedpool *) 0, size, alignment, flags); }