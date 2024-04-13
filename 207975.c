int    nedpmallopt(nedpool *p, int parno, int value) THROWSPEC
{
#if USE_ALLOCATOR==1
	return mspace_mallopt(parno, value);
#else
	return 0;
#endif
}