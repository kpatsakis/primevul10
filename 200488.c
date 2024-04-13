void path_get(struct path *path)
{
	mntget(path->mnt);
	dget(path->dentry);
}