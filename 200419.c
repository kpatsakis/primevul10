asmlinkage long sys_rmdir(const char __user *pathname)
{
	return do_rmdir(AT_FDCWD, pathname);
}