static void DestroyCaches(nedpool *RESTRICT p) THROWSPEC
{
	if(p->caches)
	{
		threadcache *tc;
		int n;
		nedflushlogs(p, 0);
		for(n=0; n<THREADCACHEMAXCACHES; n++)
		{
			if((tc=p->caches[n]))
			{
				tc->mymspace=-1;
				tc->threadid=0;
				CallFree(0, tc, 0);
				p->caches[n]=0;
			}
		}
	}
}