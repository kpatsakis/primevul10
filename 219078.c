static bool path_connected(const struct path *path)
{
	struct vfsmount *mnt = path->mnt;
	struct super_block *sb = mnt->mnt_sb;

	/* Bind mounts and multi-root filesystems can have disconnected paths */
	if (!(sb->s_iflags & SB_I_MULTIROOT) && (mnt->mnt_root == sb->s_root))
		return true;

	return is_subdir(path->dentry, mnt->mnt_root);
}