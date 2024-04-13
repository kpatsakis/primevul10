SYSCALL_DEFINE5(linkat, int, olddfd, const char __user *, oldname,
		int, newdfd, const char __user *, newname, int, flags)
{
	return do_linkat(olddfd, oldname, newdfd, newname, flags);
}