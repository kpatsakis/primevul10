static FORCEINLINE logentry *LogOperation(threadcache *tc, nedpool *np, LogEntryType type, int mspace, size_t size, void *mem, size_t alignment, unsigned flags, void *returned) THROWSPEC
{
#if ENABLE_LOGGING
	if(tc->logentries && NEDMALLOC_TESTLOGENTRY(tc, np, type, mspace, size, mem, alignment, flags, returned))
	{
		logentry *le;
		if(tc->logentriesptr==tc->logentriesend)
		{
			mchunkptr cp=mem2chunk(tc->logentries);
			size_t logentrieslen=chunksize(cp)-overhead_for(cp);
			le=(logentry *) CallRealloc(0, tc->logentries, 0, logentrieslen, (logentrieslen*3)/2, 0, M2_ZERO_MEMORY|M2_ALWAYS_MMAP|M2_RESERVE_MULT(8));
			if(!le) return 0;
			tc->logentriesptr=le+(tc->logentriesptr-tc->logentries);
			tc->logentries=le;
			cp=mem2chunk(tc->logentries);
			logentrieslen=(chunksize(cp)-overhead_for(cp))/sizeof(logentry);
			tc->logentriesend=tc->logentries+logentrieslen;
		}
		le=tc->logentriesptr++;
		assert(le+1<=tc->logentriesend);
		le->timestamp=GetTimestamp();
		le->np=np;
		le->type=type;
		le->mspace=mspace;
		le->size=size;
		le->mem=mem;
		le->alignment=alignment;
		le->flags=flags;
		le->returned=returned;
#if NEDMALLOC_STACKBACKTRACEDEPTH
		DoStackWalk(le);
#endif
		return le;
	}
#endif
	return 0;
}