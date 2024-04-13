static NEDMALLOCNOALIASATTR mstate nedblkmstate(void *RESTRICT mem) THROWSPEC
{
	if(mem)
	{
#if USE_MAGIC_HEADERS
		size_t *_mem=(size_t *) mem-3;
		if(_mem[0]==*(size_t *) "NEDMALOC")
		{
			return (mstate) _mem[1];
		}
		else return 0;
#else
#if USE_ALLOCATOR==0
		/* Fail everything */
		return 0;
#elif USE_ALLOCATOR==1
#ifdef ENABLE_FAST_HEAP_DETECTION
#ifdef WIN32
		/*  On Windows for RELEASE both x86 and x64 the NT heap precedes each block with an eight byte header
			which looks like:
				normal: 4 bytes of size, 4 bytes of [char < 64, char < 64, char < 64 bit 0 always set, char random ]
				mmaped: 4 bytes of size  4 bytes of [zero,      zero,      0xb,                        zero        ]

			On Windows for DEBUG both x86 and x64 the preceding four bytes is always 0xfdfdfdfd (no man's land).
		*/
#pragma pack(push, 1)
		struct _HEAP_ENTRY
		{
			USHORT Size;
			USHORT PreviousSize;
			UCHAR Cookie;			/* SegmentIndex */
			UCHAR Flags;			/* always bit 0 (HEAP_ENTRY_BUSY). bit 1=(HEAP_ENTRY_EXTRA_PRESENT), bit 2=normal block (HEAP_ENTRY_FILL_PATTERN), bit 3=mmap block (HEAP_ENTRY_VIRTUAL_ALLOC). Bit 4 (HEAP_ENTRY_LAST_ENTRY) could be set */
			UCHAR UnusedBytes;
			UCHAR SmallTagIndex;	/* fastbin index. Always one of 0x02, 0x03, 0x04 < 0x80 */
		} *RESTRICT he=((struct _HEAP_ENTRY *) mem)-1;
#pragma pack(pop)
		unsigned int header=((unsigned int *)mem)[-1], mask1=0x8080E100, result1, mask2=0xFFFFFF06, result2;
		result1=header & mask1;	/* Positive testing for NT heap */
		result2=header & mask2;	/* Positive testing for dlmalloc */
		if(result1==0x00000100 && result2!=0x00000102)
		{	/* This is likely a NT heap block */
			return 0;
		}
#endif
#ifdef __linux__
		/* On Linux glibc uses ptmalloc2 (really dlmalloc) just as we do, but prev_foot contains rubbish
		when the preceding block is allocated because ptmalloc2 finds the local mstate by rounding the ptr
		down to the nearest megabyte. It's like dlmalloc with FOOTERS disabled. */
		mchunkptr p=mem2chunk(mem);
		mstate fm=get_mstate_for(p);
		/* If it's a ptmalloc2 block, fm is likely to be some crazy value */
		if(!is_aligned(fm)) return 0;
		if((size_t)mem-(size_t)fm>=(size_t)1<<(SIZE_T_BITSIZE-1)) return 0;
		if(ok_magic(fm))
			return fm;
		else
			return 0;
		if(1) { }
#endif
		else
		{
			mchunkptr p=mem2chunk(mem);
			mstate fm=get_mstate_for(p);
			assert(ok_magic(fm));	/* If this fails, someone tried to free a block twice */
			if(ok_magic(fm))
				return fm;
		}
#else
#ifdef WIN32
#ifdef _MSC_VER
		__try
#elif defined(__MINGW32__)
		__try1
#endif
#endif
		{
			/* We try to return zero here if it isn't one of our own blocks, however
			the current block annotation scheme used by dlmalloc makes it impossible
			to be absolutely sure of avoiding a segfault.

			mchunkptr->prev_foot = mem-(2*size_t) = mstate ^ mparams.magic for PRECEDING block;
			mchunkptr->head      = mem-(1*size_t) = 8 multiple size of this block with bottom three bits = FLAG_BITS
			    FLAG_BITS = bit 0 is CINUSE (currently in use unless is mmap), bit 1 is PINUSE (previous block currently
				            in use unless mmap), bit 2 is UNUSED and currently is always zero.
			*/
			register void *RESTRICT leastusedaddress_=leastusedaddress;		/* Cache these to avoid register reloading */
			register size_t largestusedblock_=largestusedblock;
			if(!is_aligned(mem)) return 0;		/* Would fail very rarely as all allocators return aligned blocks */
			if(mem<leastusedaddress_) return 0;	/* Simple but effective */
			{
				mchunkptr p=mem2chunk(mem);
				mstate fm=0;
				int ismmapped=is_mmapped(p);
				if((!ismmapped && !is_inuse(p)) || (p->head & FLAG4_BIT)) return 0;
				/* Reduced uncertainty by 0.5^2 = 25.0% */
				/* size should never exceed largestusedblock */
				if(chunksize(p)-overhead_for(p)>largestusedblock_) return 0;
				/* Reduced uncertainty by a minimum of 0.5^3 = 12.5%, maximum 0.5^16 = 0.0015% */
				/* Having sanity checked prev_foot and head, check next block */
				if(!ismmapped && (!next_pinuse(p) || (next_chunk(p)->head & FLAG4_BIT))) return 0;
				/* Reduced uncertainty by 0.5^5 = 3.13% or 0.5^18 = 0.00038% */
#if 0
				/* If previous block is free, check that its next block pointer equals us */
				if(!ismmapped && !pinuse(p))
					if(next_chunk(prev_chunk(p))!=p) return 0;
				/* We could start comparing prev_foot's for similarity but it starts getting slow. */
#endif
				fm = get_mstate_for(p);
				if(!is_aligned(fm) || (void *)fm<leastusedaddress_) return 0;
#if 0
				/* See if mem is lower in memory than mem */
				if((size_t)mem-(size_t)fm>=(size_t)1<<(SIZE_T_BITSIZE-1)) return 0;
#endif
				assert(ok_magic(fm));	/* If this fails, someone tried to free a block twice */
				if(ok_magic(fm))
					return fm;
			}
		}
#ifdef WIN32
#ifdef _MSC_VER
		__except(1) { }
#elif defined(__MINGW32__)
		__except1(1) { }
#endif
#endif
#endif
#endif
#endif
	}
	return 0;
}