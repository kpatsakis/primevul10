int follow_down(struct vfsmount **mnt, struct dentry **dentry)
{
	struct vfsmount *mounted;

	mounted = lookup_mnt(*mnt, *dentry);
	if (mounted) {
		dput(*dentry);
		mntput(*mnt);
		*mnt = mounted;
		*dentry = dget(mounted->mnt_root);
		return 1;
	}
	return 0;
}