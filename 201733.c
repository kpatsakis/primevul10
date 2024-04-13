static int sig_ignored(struct task_struct *t, int sig, int from_ancestor_ns)
{
	/*
	 * Blocked signals are never ignored, since the
	 * signal handler may change by the time it is
	 * unblocked.
	 */
	if (sigismember(&t->blocked, sig) || sigismember(&t->real_blocked, sig))
		return 0;

	if (!sig_task_ignored(t, sig, from_ancestor_ns))
		return 0;

	/*
	 * Tracers may want to know about even ignored signals.
	 */
	return !tracehook_consider_ignored_signal(t, sig);
}