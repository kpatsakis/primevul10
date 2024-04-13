int __user_walk(const char __user *name, unsigned flags, struct nameidata *nd)
{
	return __user_walk_fd(AT_FDCWD, name, flags, nd);
}