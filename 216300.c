R_API int r_sys_usleep(int usecs) {
#if HAS_CLOCK_NANOSLEEP
	struct timespec rqtp;
	rqtp.tv_sec = usecs / 1000000;
	rqtp.tv_nsec = (usecs - (rqtp.tv_sec * 1000000)) * 1000;
	return clock_nanosleep (CLOCK_MONOTONIC, 0, &rqtp, NULL);
#elif __UNIX__
	return usleep (usecs);
#else
	// w32 api uses milliseconds
	usecs /= 1000;
	Sleep (usecs); // W32
	return 0;
#endif
}