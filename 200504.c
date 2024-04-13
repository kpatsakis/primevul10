asmlinkage long sys_symlink(const char __user *oldname, const char __user *newname)
{
	return sys_symlinkat(oldname, AT_FDCWD, newname);
}