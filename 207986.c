NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR void * nedmemalign(size_t alignment, size_t bytes) THROWSPEC								{ return nedpmemalign((nedpool *) 0, alignment, bytes); }