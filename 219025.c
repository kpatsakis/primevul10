SYSCALL_DEFINE5(renameat2, int, olddfd, const char __user *, oldname,
		int, newdfd, const char __user *, newname, unsigned int, flags)
{
	return do_renameat2(olddfd, oldname, newdfd, newname, flags);
}