SYSCALL_DEFINE4(renameat, int, olddfd, const char __user *, oldname,
		int, newdfd, const char __user *, newname)
{
	return do_renameat2(olddfd, oldname, newdfd, newname, 0);
}