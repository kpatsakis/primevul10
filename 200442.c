asmlinkage long sys_renameat(int olddfd, const char __user *oldname,
			     int newdfd, const char __user *newname)
{
	int error;
	char * from;
	char * to;

	from = getname(oldname);
	if(IS_ERR(from))
		return PTR_ERR(from);
	to = getname(newname);
	error = PTR_ERR(to);
	if (!IS_ERR(to)) {
		error = do_rename(olddfd, from, newdfd, to);
		putname(to);
	}
	putname(from);
	return error;
}