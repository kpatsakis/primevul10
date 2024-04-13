static timeCount GetTimestamp()
{
	static timeCount baseCount;
	timeCount ret;
#ifdef CLOCK_MONOTONIC
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	ret=((timeCount) ts.tv_sec*1000000000000LL)+ts.tv_nsec*1000LL;
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	ret=((timeCount) tv.tv_sec*1000000000000LL)+tv.tv_usec*1000000LL;
#endif
	if(!baseCount) baseCount=ret;
	return ret-baseCount;
}