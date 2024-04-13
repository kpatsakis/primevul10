do_send_specific(pid_t tgid, pid_t pid, int sig, struct siginfo *info)
{
	struct task_struct *p;
	unsigned long flags;
	int error = -ESRCH;

	rcu_read_lock();
	p = find_task_by_vpid(pid);
	if (p && (tgid <= 0 || task_tgid_vnr(p) == tgid)) {
		error = check_kill_permission(sig, info, p);
		/*
		 * The null signal is a permissions and process existence
		 * probe.  No signal is actually delivered.
		 *
		 * If lock_task_sighand() fails we pretend the task dies
		 * after receiving the signal. The window is tiny, and the
		 * signal is private anyway.
		 */
		if (!error && sig && lock_task_sighand(p, &flags)) {
			error = specific_send_sig_info(sig, info, p);
			unlock_task_sighand(p, &flags);
		}
	}
	rcu_read_unlock();

	return error;
}