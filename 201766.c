void recalc_sigpending(void)
{
	if (unlikely(tracehook_force_sigpending()))
		set_thread_flag(TIF_SIGPENDING);
	else if (!recalc_sigpending_tsk(current) && !freezing(current))
		clear_thread_flag(TIF_SIGPENDING);

}