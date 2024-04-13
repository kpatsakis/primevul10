inotify_get_sb(struct file_system_type *fs_type, int flags,
	       const char *dev_name, void *data, struct vfsmount *mnt)
{
	return get_sb_pseudo(fs_type, "inotify", NULL,
			INOTIFYFS_SUPER_MAGIC, mnt);
}