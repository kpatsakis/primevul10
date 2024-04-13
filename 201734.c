void exit_signals(struct task_struct *tsk)
{
	int group_stop = 0;
	struct task_struct *t;

	if (thread_group_empty(tsk) || signal_group_exit(tsk->signal)) {
		tsk->flags |= PF_EXITING;
		return;
	}

	spin_lock_irq(&tsk->sighand->siglock);
	/*
	 * From now this task is not visible for group-wide signals,
	 * see wants_signal(), do_signal_stop().
	 */
	tsk->flags |= PF_EXITING;
	if (!signal_pending(tsk))
		goto out;

	/* It could be that __group_complete_signal() choose us to
	 * notify about group-wide signal. Another thread should be
	 * woken now to take the signal since we will not.
	 */
	for (t = tsk; (t = next_thread(t)) != tsk; )
		if (!signal_pending(t) && !(t->flags & PF_EXITING))
			recalc_sigpending_and_wake(t);

	if (unlikely(tsk->signal->group_stop_count) &&
			!--tsk->signal->group_stop_count) {
		tsk->signal->flags = SIGNAL_STOP_STOPPED;
		group_stop = 1;
	}
out:
	spin_unlock_irq(&tsk->sighand->siglock);

	if (unlikely(group_stop) && tracehook_notify_jctl(1, CLD_STOPPED)) {
		read_lock(&tasklist_lock);
		do_notify_parent_cldstop(tsk, CLD_STOPPED);
		read_unlock(&tasklist_lock);
	}
}