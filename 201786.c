int kill_pid_info_as_uid(int sig, struct siginfo *info, struct pid *pid,
		      uid_t uid, uid_t euid, u32 secid)
{
	int ret = -EINVAL;
	struct task_struct *p;
	const struct cred *pcred;

	if (!valid_signal(sig))
		return ret;

	read_lock(&tasklist_lock);
	p = pid_task(pid, PIDTYPE_PID);
	if (!p) {
		ret = -ESRCH;
		goto out_unlock;
	}
	pcred = __task_cred(p);
	if ((info == SEND_SIG_NOINFO ||
	     (!is_si_special(info) && SI_FROMUSER(info))) &&
	    euid != pcred->suid && euid != pcred->uid &&
	    uid  != pcred->suid && uid  != pcred->uid) {
		ret = -EPERM;
		goto out_unlock;
	}
	ret = security_task_kill(p, info, sig, secid);
	if (ret)
		goto out_unlock;
	if (sig && p->sighand) {
		unsigned long flags;
		spin_lock_irqsave(&p->sighand->siglock, flags);
		ret = __send_signal(sig, info, p, 1, 0);
		spin_unlock_irqrestore(&p->sighand->siglock, flags);
	}
out_unlock:
	read_unlock(&tasklist_lock);
	return ret;
}