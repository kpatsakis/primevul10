static int sig_task_ignored(struct task_struct *t, int sig,
		int from_ancestor_ns)
{
	void __user *handler;

	handler = sig_handler(t, sig);

	if (unlikely(t->signal->flags & SIGNAL_UNKILLABLE) &&
			handler == SIG_DFL && !from_ancestor_ns)
		return 1;

	return sig_handler_ignored(handler, sig);
}