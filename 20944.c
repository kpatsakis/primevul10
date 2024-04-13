mono_time(void) {
	struct timeval ret;
#if defined(HAVE_CLOCK_GETTIME) && defined(CLOCK_MONOTONIC)
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
		ret.tv_sec = ts.tv_sec;
		ret.tv_usec = ts.tv_nsec/1000;
		return ret;
	}
#endif
	gettimeofday(&ret,NULL);
	return ret;
}