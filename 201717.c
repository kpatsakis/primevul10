int group_send_sig_info(int sig, struct siginfo *info, struct task_struct *p)
{
	unsigned long flags;
	int ret;

	ret = check_kill_permission(sig, info, p);

	if (!ret && sig) {
		ret = -ESRCH;
		if (lock_task_sighand(p, &flags)) {
			ret = __group_send_sig_info(sig, info, p);
			unlock_task_sighand(p, &flags);
		}
	}

	return ret;
}