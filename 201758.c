SYSCALL_DEFINE2(rt_sigpending, sigset_t __user *, set, size_t, sigsetsize)
{
	return do_sigpending(set, sigsetsize);
}