nedpool **nedpoollist() THROWSPEC
{
	nedpool **ret=0;
	if(poollist)
	{
#if USE_LOCKS
		ACQUIRE_LOCK(&poollistlock);
#endif
		if(!(ret=(nedpool **) nedmalloc((poollist->length+1)*sizeof(nedpool *)))) goto badexit;
		memcpy(ret, poollist->list, (poollist->length+1)*sizeof(nedpool *));
badexit:
		{
#if USE_LOCKS
			RELEASE_LOCK(&poollistlock);
#endif
		}
	}
	return ret;
}