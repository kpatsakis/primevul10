force_sig_specific(int sig, struct task_struct *t)
{
	force_sig_info(sig, SEND_SIG_FORCED, t);
}