R_API int r_sys_sleep(int secs) {
#if HAS_CLOCK_NANOSLEEP
	struct timespec rqtp;
	rqtp.tv_sec = secs;
	rqtp.tv_nsec = 0;
	return clock_nanosleep (CLOCK_MONOTONIC, 0, &rqtp, NULL);
#elif __UNIX__
	return sleep (secs);
#else
	Sleep (secs * 1000); // W32
	return 0;
#endif
}