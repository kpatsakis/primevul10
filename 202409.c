static int psched_show(struct seq_file *seq, void *v)
{
	seq_printf(seq, "%08x %08x %08x %08x\n",
		      psched_tick_per_us, psched_us_per_tick,
		      1000000, HZ);

	return 0;
}