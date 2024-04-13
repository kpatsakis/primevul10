static int ptrace_signal(int signr, siginfo_t *info,
			 struct pt_regs *regs, void *cookie)
{
	if (!task_ptrace(current))
		return signr;

	ptrace_signal_deliver(regs, cookie);

	/* Let the debugger run.  */
	ptrace_stop(signr, 0, info);

	/* We're back.  Did the debugger cancel the sig?  */
	signr = current->exit_code;
	if (signr == 0)
		return signr;

	current->exit_code = 0;

	/* Update the siginfo structure if the signal has
	   changed.  If the debugger wanted something
	   specific in the siginfo structure then it should
	   have updated *info via PTRACE_SETSIGINFO.  */
	if (signr != info->si_signo) {
		info->si_signo = signr;
		info->si_errno = 0;
		info->si_code = SI_USER;
		info->si_pid = task_pid_vnr(current->parent);
		info->si_uid = task_uid(current->parent);
	}

	/* If the (new) signal is now blocked, requeue it.  */
	if (sigismember(&current->blocked, signr)) {
		specific_send_sig_info(signr, info, current);
		signr = 0;
	}

	return signr;
}