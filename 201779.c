SYSCALL_DEFINE1(sigpending, old_sigset_t __user *, set)
{
	return do_sigpending(set, sizeof(*set));
}