static void psched_tick(unsigned long dummy)
{
	if (sizeof(cycles_t) == sizeof(u32)) {
		psched_time_t dummy_stamp;
		PSCHED_GET_TIME(dummy_stamp);
		psched_timer.expires = jiffies + 1*HZ;
		add_timer(&psched_timer);
	}
}