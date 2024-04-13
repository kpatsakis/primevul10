static int check_kill_permission(int sig, struct siginfo *info,
				 struct task_struct *t)
{
	const struct cred *cred = current_cred(), *tcred;
	struct pid *sid;
	int error;

	if (!valid_signal(sig))
		return -EINVAL;

	if (info != SEND_SIG_NOINFO && (is_si_special(info) || SI_FROMKERNEL(info)))
		return 0;

	error = audit_signal_info(sig, t); /* Let audit system see the signal */
	if (error)
		return error;

	tcred = __task_cred(t);
	if ((cred->euid ^ tcred->suid) &&
	    (cred->euid ^ tcred->uid) &&
	    (cred->uid  ^ tcred->suid) &&
	    (cred->uid  ^ tcred->uid) &&
	    !capable(CAP_KILL)) {
		switch (sig) {
		case SIGCONT:
			sid = task_session(t);
			/*
			 * We don't return the error if sid == NULL. The
			 * task was unhashed, the caller must notice this.
			 */
			if (!sid || sid == task_session(current))
				break;
		default:
			return -EPERM;
		}
	}

	return security_task_kill(t, info, sig, 0);
}