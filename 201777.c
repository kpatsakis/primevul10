int __fatal_signal_pending(struct task_struct *tsk)
{
	return sigismember(&tsk->pending.signal, SIGKILL);
}