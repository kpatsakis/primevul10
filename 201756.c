static int do_signal_stop(int signr)
{
	struct signal_struct *sig = current->signal;
	int stop_count;

	if (sig->group_stop_count > 0) {
		/*
		 * There is a group stop in progress.  We don't need to
		 * start another one.
		 */
		stop_count = --sig->group_stop_count;
	} else {
		struct task_struct *t;

		if (!likely(sig->flags & SIGNAL_STOP_DEQUEUED) ||
		    unlikely(signal_group_exit(sig)))
			return 0;
		/*
		 * There is no group stop already in progress.
		 * We must initiate one now.
		 */
		sig->group_exit_code = signr;

		stop_count = 0;
		for (t = next_thread(current); t != current; t = next_thread(t))
			/*
			 * Setting state to TASK_STOPPED for a group
			 * stop is always done with the siglock held,
			 * so this check has no races.
			 */
			if (!(t->flags & PF_EXITING) &&
			    !task_is_stopped_or_traced(t)) {
				stop_count++;
				signal_wake_up(t, 0);
			}
		sig->group_stop_count = stop_count;
	}

	if (stop_count == 0)
		sig->flags = SIGNAL_STOP_STOPPED;
	current->exit_code = sig->group_exit_code;
	__set_current_state(TASK_STOPPED);

	spin_unlock_irq(&current->sighand->siglock);
	finish_stop(stop_count);
	return 1;
}