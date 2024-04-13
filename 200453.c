asmlinkage long sys_rename(const char __user *oldname, const char __user *newname)
{
	return sys_renameat(AT_FDCWD, oldname, AT_FDCWD, newname);
}