static int send_signal(int sig, struct siginfo *info, struct task_struct *t,
			int group)
{
	int from_ancestor_ns = 0;

#ifdef CONFIG_PID_NS
	if (!is_si_special(info) && SI_FROMUSER(info) &&
			task_pid_nr_ns(current, task_active_pid_ns(t)) <= 0)
		from_ancestor_ns = 1;
#endif

	return __send_signal(sig, info, t, group, from_ancestor_ns);
}