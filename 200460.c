static void follow_mount(struct vfsmount **mnt, struct dentry **dentry)
{
	while (d_mountpoint(*dentry)) {
		struct vfsmount *mounted = lookup_mnt(*mnt, *dentry);
		if (!mounted)
			break;
		dput(*dentry);
		mntput(*mnt);
		*mnt = mounted;
		*dentry = dget(mounted->mnt_root);
	}
}