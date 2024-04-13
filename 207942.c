NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR void * nedprealloc(nedpool *p, void *mem, size_t size) THROWSPEC
{
	unsigned flags=NEDMALLOC_FORCERESERVE(p, mem, size);
#if ENABLE_USERMODEPAGEALLOCATOR
	/* If the user mode page allocator is turned on in a 32 bit process,
	don't automatically reserve eight times the address space. */
	if(8==sizeof(size_t) || !OSHavePhysicalPageSupport())
#endif
	{	/* If he reallocs even once, it's probably wise to turn on address space reservation.
		If the size is larger than mmap_threshold then it'll set the reserve. */
		if(!(flags & M2_RESERVE_MASK)) flags=M2_RESERVE_MULT(8);
	}
	return nedprealloc2(p, mem, size, 0, flags);
}