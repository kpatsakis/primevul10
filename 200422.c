asmlinkage long sys_mknod(const char __user *filename, int mode, unsigned dev)
{
	return sys_mknodat(AT_FDCWD, filename, mode, dev);
}