NEDMALLOCNOALIASATTR NEDMALLOCPTRATTR nedpool *nedcreatepool(size_t capacity, int threads) THROWSPEC
{
	nedpool *ret=0;
	if(!poollist)
	{
		PoolList *newpoollist=0;
		if(!(newpoollist=(PoolList *) nedpcalloc(0, 1, sizeof(PoolList)+sizeof(nedpool *)))) return 0;
#if USE_LOCKS
		INITIAL_LOCK(&poollistlock);
		ACQUIRE_LOCK(&poollistlock);
#endif
		poollist=newpoollist;
		poollist->size=sizeof(poollist->list)/sizeof(nedpool *);
	}
#if USE_LOCKS
	else
		ACQUIRE_LOCK(&poollistlock);
#endif
	if(poollist->length==poollist->size)
	{
		PoolList *newpoollist=0;
		size_t newsize=0;
		newsize=sizeof(PoolList)+(poollist->size+1)*sizeof(nedpool *);
		if(!(newpoollist=(PoolList *) nedprealloc(0, poollist, newsize))) goto badexit;
		poollist=newpoollist;
		memset(&poollist->list[poollist->size], 0, newsize-((size_t)&poollist->list[poollist->size]-(size_t)&poollist->list[0]));
		poollist->size=((newsize-((char *)&poollist->list[0]-(char *)poollist))/sizeof(nedpool *))-1;
		assert(poollist->size>poollist->length);
	}
	if(!(ret=(nedpool *) nedpcalloc(0, 1, sizeof(nedpool)))) goto badexit;
	if(!InitPool(ret, capacity, threads))
	{
		nedpfree(0, ret);
		goto badexit;
	}
	poollist->list[poollist->length++]=ret;
badexit:
	{
#if USE_LOCKS
		RELEASE_LOCK(&poollistlock);
#endif
	}
	return ret;
}