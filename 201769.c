int get_signal_to_deliver(siginfo_t *info, struct k_sigaction *return_ka,
			  struct pt_regs *regs, void *cookie)
{
	struct sighand_struct *sighand = current->sighand;
	struct signal_struct *signal = current->signal;
	int signr;

relock:
	/*
	 * We'll jump back here after any time we were stopped in TASK_STOPPED.
	 * While in TASK_STOPPED, we were considered "frozen enough".
	 * Now that we woke up, it's crucial if we're supposed to be
	 * frozen that we freeze now before running anything substantial.
	 */
	try_to_freeze();

	spin_lock_irq(&sighand->siglock);
	/*
	 * Every stopped thread goes here after wakeup. Check to see if
	 * we should notify the parent, prepare_signal(SIGCONT) encodes
	 * the CLD_ si_code into SIGNAL_CLD_MASK bits.
	 */
	if (unlikely(signal->flags & SIGNAL_CLD_MASK)) {
		int why = (signal->flags & SIGNAL_STOP_CONTINUED)
				? CLD_CONTINUED : CLD_STOPPED;
		signal->flags &= ~SIGNAL_CLD_MASK;
		spin_unlock_irq(&sighand->siglock);

		if (unlikely(!tracehook_notify_jctl(1, why)))
			goto relock;

		read_lock(&tasklist_lock);
		do_notify_parent_cldstop(current->group_leader, why);
		read_unlock(&tasklist_lock);
		goto relock;
	}

	for (;;) {
		struct k_sigaction *ka;

		if (unlikely(signal->group_stop_count > 0) &&
		    do_signal_stop(0))
			goto relock;

		/*
		 * Tracing can induce an artifical signal and choose sigaction.
		 * The return value in @signr determines the default action,
		 * but @info->si_signo is the signal number we will report.
		 */
		signr = tracehook_get_signal(current, regs, info, return_ka);
		if (unlikely(signr < 0))
			goto relock;
		if (unlikely(signr != 0))
			ka = return_ka;
		else {
			signr = dequeue_signal(current, &current->blocked,
					       info);

			if (!signr)
				break; /* will return 0 */

			if (signr != SIGKILL) {
				signr = ptrace_signal(signr, info,
						      regs, cookie);
				if (!signr)
					continue;
			}

			ka = &sighand->action[signr-1];
		}

		if (ka->sa.sa_handler == SIG_IGN) /* Do nothing.  */
			continue;
		if (ka->sa.sa_handler != SIG_DFL) {
			/* Run the handler.  */
			*return_ka = *ka;

			if (ka->sa.sa_flags & SA_ONESHOT)
				ka->sa.sa_handler = SIG_DFL;

			break; /* will return non-zero "signr" value */
		}

		/*
		 * Now we are doing the default action for this signal.
		 */
		if (sig_kernel_ignore(signr)) /* Default is nothing. */
			continue;

		/*
		 * Global init gets no signals it doesn't want.
		 * Container-init gets no signals it doesn't want from same
		 * container.
		 *
		 * Note that if global/container-init sees a sig_kernel_only()
		 * signal here, the signal must have been generated internally
		 * or must have come from an ancestor namespace. In either
		 * case, the signal cannot be dropped.
		 */
		if (unlikely(signal->flags & SIGNAL_UNKILLABLE) &&
				!sig_kernel_only(signr))
			continue;

		if (sig_kernel_stop(signr)) {
			/*
			 * The default action is to stop all threads in
			 * the thread group.  The job control signals
			 * do nothing in an orphaned pgrp, but SIGSTOP
			 * always works.  Note that siglock needs to be
			 * dropped during the call to is_orphaned_pgrp()
			 * because of lock ordering with tasklist_lock.
			 * This allows an intervening SIGCONT to be posted.
			 * We need to check for that and bail out if necessary.
			 */
			if (signr != SIGSTOP) {
				spin_unlock_irq(&sighand->siglock);

				/* signals can be posted during this window */

				if (is_current_pgrp_orphaned())
					goto relock;

				spin_lock_irq(&sighand->siglock);
			}

			if (likely(do_signal_stop(info->si_signo))) {
				/* It released the siglock.  */
				goto relock;
			}

			/*
			 * We didn't actually stop, due to a race
			 * with SIGCONT or something like that.
			 */
			continue;
		}

		spin_unlock_irq(&sighand->siglock);

		/*
		 * Anything else is fatal, maybe with a core dump.
		 */
		current->flags |= PF_SIGNALED;

		if (sig_kernel_coredump(signr)) {
			if (print_fatal_signals)
				print_fatal_signal(regs, info->si_signo);
			/*
			 * If it was able to dump core, this kills all
			 * other threads in the group and synchronizes with
			 * their demise.  If we lost the race with another
			 * thread getting here, it set group_exit_code
			 * first and our do_group_exit call below will use
			 * that value and ignore the one we pass it.
			 */
			do_coredump(info->si_signo, info->si_signo, regs);
		}

		/*
		 * Death signals, no core dump.
		 */
		do_group_exit(info->si_signo);
		/* NOTREACHED */
	}
	spin_unlock_irq(&sighand->siglock);
	return signr;
}