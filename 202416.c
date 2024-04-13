int __init psched_calibrate_clock(void)
{
	psched_time_t stamp, stamp1;
	struct timeval tv, tv1;
	psched_tdiff_t delay;
	long rdelay;
	unsigned long stop;

	psched_tick(0);
	stop = jiffies + HZ/10;
	PSCHED_GET_TIME(stamp);
	do_gettimeofday(&tv);
	while (time_before(jiffies, stop)) {
		barrier();
		cpu_relax();
	}
	PSCHED_GET_TIME(stamp1);
	do_gettimeofday(&tv1);

	delay = PSCHED_TDIFF(stamp1, stamp);
	rdelay = tv1.tv_usec - tv.tv_usec;
	rdelay += (tv1.tv_sec - tv.tv_sec)*1000000;
	if (rdelay > delay)
		return -1;
	delay /= rdelay;
	psched_tick_per_us = delay;
	while ((delay>>=1) != 0)
		psched_clock_scale++;
	psched_us_per_tick = 1<<psched_clock_scale;
	psched_clock_per_hz = (psched_tick_per_us*(1000000/HZ))>>psched_clock_scale;
	return 0;
}