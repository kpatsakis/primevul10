static void tcfullsanitycheck(threadcache *tc) THROWSPEC
{
	threadcacheblk *RESTRICT *RESTRICT tcbptr=tc->bins;
	int n;
	for(n=0; n<=THREADCACHEMAXBINS; n++, tcbptr+=2)
	{
		threadcacheblk *RESTRICT b, *RESTRICT ob=0;
		tcsanitycheck(tcbptr);
		for(b=tcbptr[0]; b; ob=b, b=b->next)
		{
			assert(b->isforeign || nedblkmstate(b));
			assert(nedblksize(0, b, 0)>=sizeof(threadcacheblk));
			assert(*(unsigned int *) "NEDN"==b->magic);
			assert(!ob || ob->next==b);
			assert(!ob || b->prev==ob);
		}
	}
}