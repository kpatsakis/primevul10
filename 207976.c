size_t nedflushlogs(nedpool *p, char *filepath) THROWSPEC
{
	size_t count=0;
	if(!p)
	{
		p=&syspool;
		if(!syspool.threads) InitPool(&syspool, 0, -1);
	}
	if(p->caches)
	{
		threadcache *tc;
		int n;
#if ENABLE_LOGGING
		int haslogentries=0;
#endif
		for(n=0; n<THREADCACHEMAXCACHES; n++)
		{
			if((tc=p->caches[n]))
			{
				count+=tc->freeInCache;
				tc->frees++;
				RemoveCacheEntries(p, tc, 0);
				assert(!tc->freeInCache);
#if ENABLE_LOGGING
				haslogentries|=!!tc->logentries;
#endif
			}
		}
#if ENABLE_LOGGING
		if(haslogentries)
		{
			char buffer[MAX_PATH]=NEDMALLOC_LOGFILE;
			FILE *oh;
			fpos_t pos1, pos2;
			if(!filepath) filepath=buffer;
			oh=fopen(filepath, "r+");
			while(!oh)
			{
				char *bptr;
				if((oh=fopen(filepath, "w"))) break;
				if(ENOSPC==errno) break;
				bptr=strrchr(filepath, '.');
				if(bptr-filepath>=MAX_PATH-6) abort();
				memcpy(bptr, "!.csv", 6);
			}
			if(oh)
			{
				fgetpos(oh, &pos1);
				fseek(oh, 0, SEEK_END);
				fgetpos(oh, &pos2);
				if(pos1==pos2)
					fprintf(oh, "Timestamp, Pool, Operation, MSpace, Size, Block, Alignment, Flags, Returned,\"Stack Backtrace\"\n");
				for(n=0; n<THREADCACHEMAXCACHES; n++)
				{
					if((tc=p->caches[n]) && tc->logentries)
					{
						logentry *le;
						for(le=tc->logentries; le<tc->logentriesptr; le++)
						{
							const char *LogEntryTypeString=LogEntryTypeStrings[size2binidx(((size_t)le->type)<<4)];
							char stackbacktrace[16384]="?";
#if NEDMALLOC_STACKBACKTRACEDEPTH
							char *sbtp=stackbacktrace;
							int i;
							for(i=0; i<NEDMALLOC_STACKBACKTRACEDEPTH && le->stack[i].pc; i++)
							{
								sbtp+=sprintf(sbtp, "0x%p:%s:%s (%s:%u),",
									le->stack[i].pc, le->stack[i].module, le->stack[i].functname, le->stack[i].file, le->stack[i].lineno);
								if(sbtp>=stackbacktrace+sizeof(stackbacktrace)) abort();
							}
							if(NEDMALLOC_STACKBACKTRACEDEPTH==i)
								strcpy(sbtp, "<backtrace may continue ...>");
							else
								strcpy(sbtp, "<backtrace ends>");
							if(strchr(sbtp, 0)>=stackbacktrace+sizeof(stackbacktrace)) abort();
#endif
							fprintf(oh, "%llu, 0x%p, %s, %d, %Iu, 0x%p, %Iu, 0x%x, 0x%p,\"%s\"\n",
								le->timestamp, le->np, LogEntryTypeString, le->mspace, le->size, le->mem, le->alignment, le->flags, le->returned, stackbacktrace);
						}
						CallFree(0, tc->logentries, 0);
						tc->logentries=tc->logentriesptr=tc->logentriesend=0;
					}
				}
				fclose(oh);
			}
		}
#endif
	}
	return count;
}