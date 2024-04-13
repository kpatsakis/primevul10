static void tcsanitycheck(threadcacheblk *RESTRICT *RESTRICT ptr) THROWSPEC
{
	assert((ptr[0] && ptr[1]) || (!ptr[0] && !ptr[1]));
	if(ptr[0] && ptr[1])
	{
		assert(ptr[0]->isforeign || nedblkmstate(ptr[0]));
		assert(ptr[1]->isforeign || nedblkmstate(ptr[1]));
		assert(nedblksize(0, ptr[0], 0)>=sizeof(threadcacheblk));
		assert(nedblksize(0, ptr[1], 0)>=sizeof(threadcacheblk));
		assert(*(unsigned int *) "NEDN"==ptr[0]->magic);
		assert(*(unsigned int *) "NEDN"==ptr[1]->magic);
		assert(!ptr[0]->prev);
		assert(!ptr[1]->next);
		if(ptr[0]==ptr[1])
		{
			assert(!ptr[0]->next);
			assert(!ptr[1]->prev);
		}
	}
}