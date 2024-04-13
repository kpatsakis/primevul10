int kill_pid_info(int sig, struct siginfo *info, struct pid *pid)
{
	int error = -ESRCH;
	struct task_struct *p;

	rcu_read_lock();
retry:
	p = pid_task(pid, PIDTYPE_PID);
	if (p) {
		error = group_send_sig_info(sig, info, p);
		if (unlikely(error == -ESRCH))
			/*
			 * The task was unhashed in between, try again.
			 * If it is dead, pid_task() will return NULL,
			 * if we race with de_thread() it will find the
			 * new leader.
			 */
			goto retry;
	}
	rcu_read_unlock();

	return error;
}