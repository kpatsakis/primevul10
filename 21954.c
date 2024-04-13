get_callchain_entry_for_task(struct task_struct *task, u32 init_nr)
{
#ifdef CONFIG_STACKTRACE
	struct perf_callchain_entry *entry;
	int rctx;

	entry = get_callchain_entry(&rctx);

	if (!entry)
		return NULL;

	entry->nr = init_nr +
		stack_trace_save_tsk(task, (unsigned long *)(entry->ip + init_nr),
				     sysctl_perf_event_max_stack - init_nr, 0);

	/* stack_trace_save_tsk() works on unsigned long array, while
	 * perf_callchain_entry uses u64 array. For 32-bit systems, it is
	 * necessary to fix this mismatch.
	 */
	if (__BITS_PER_LONG != 64) {
		unsigned long *from = (unsigned long *) entry->ip;
		u64 *to = entry->ip;
		int i;

		/* copy data from the end to avoid using extra buffer */
		for (i = entry->nr - 1; i >= (int)init_nr; i--)
			to[i] = (u64)(from[i]);
	}

	put_callchain_entry(rctx);

	return entry;
#else /* CONFIG_STACKTRACE */
	return NULL;
#endif
}