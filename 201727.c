finish_stop(int stop_count)
{
	/*
	 * If there are no other threads in the group, or if there is
	 * a group stop in progress and we are the last to stop,
	 * report to the parent.  When ptraced, every thread reports itself.
	 */
	if (tracehook_notify_jctl(stop_count == 0, CLD_STOPPED)) {
		read_lock(&tasklist_lock);
		do_notify_parent_cldstop(current, CLD_STOPPED);
		read_unlock(&tasklist_lock);
	}

	do {
		schedule();
	} while (try_to_freeze());
	/*
	 * Now we don't run again until continued.
	 */
	current->exit_code = 0;
}