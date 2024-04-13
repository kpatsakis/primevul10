asmlinkage long sys_mkdir(const char __user *pathname, int mode)
{
	return sys_mkdirat(AT_FDCWD, pathname, mode);
}