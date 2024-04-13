static NOINLINE void RemoveCacheEntries(nedpool *RESTRICT p, threadcache *RESTRICT tc, unsigned int age) THROWSPEC
{
#ifdef FULLSANITYCHECKS
	tcfullsanitycheck(tc);
#endif
	if(tc->freeInCache)
	{
		threadcacheblk *RESTRICT *RESTRICT tcbptr=tc->bins;
		int n;
		for(n=0; n<=THREADCACHEMAXBINS; n++, tcbptr+=2)
		{
			threadcacheblk *RESTRICT *RESTRICT tcb=tcbptr+1;		/* come from oldest end of list */
			/*tcsanitycheck(tcbptr);*/
			for(; *tcb && tc->frees-(*tcb)->lastUsed>=age; )
			{
				threadcacheblk *RESTRICT f=*tcb;
				size_t blksize=f->size; /*nedblksize(f);*/
				assert(blksize<=nedblksize(0, f, 0));
				assert(blksize);
#ifdef FULLSANITYCHECKS
				assert(*(unsigned int *) "NEDN"==(*tcb)->magic);
#endif
				*tcb=(*tcb)->prev;
				if(*tcb)
					(*tcb)->next=0;
				else
					*tcbptr=0;
				tc->freeInCache-=blksize;
				assert((long) tc->freeInCache>=0);
				CallFree(0, f, f->isforeign);
				/*tcsanitycheck(tcbptr);*/
				LogOperation(tc, p, LOGENTRY_THREADCACHE_CLEAN, age, blksize, f, 0, 0, 0);
			}
		}
	}
#ifdef FULLSANITYCHECKS
	tcfullsanitycheck(tc);
#endif
}