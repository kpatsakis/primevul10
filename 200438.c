asmlinkage long sys_unlink(const char __user *pathname)
{
	return do_unlinkat(AT_FDCWD, pathname);
}