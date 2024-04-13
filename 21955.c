static __u64 count_kernel_ip(struct perf_callchain_entry *trace)
{
	__u64 nr_kernel = 0;

	while (nr_kernel < trace->nr) {
		if (trace->ip[nr_kernel] == PERF_CONTEXT_USER)
			break;
		nr_kernel++;
	}
	return nr_kernel;
}