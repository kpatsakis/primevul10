static int may_linkat(struct path *link)
{
	struct inode *inode = link->dentry->d_inode;

	/* Inode writeback is not safe when the uid or gid are invalid. */
	if (!uid_valid(inode->i_uid) || !gid_valid(inode->i_gid))
		return -EOVERFLOW;

	if (!sysctl_protected_hardlinks)
		return 0;

	/* Source inode owner (or CAP_FOWNER) can hardlink all they like,
	 * otherwise, it must be a safe source.
	 */
	if (safe_hardlink_source(inode) || inode_owner_or_capable(inode))
		return 0;

	audit_log_path_denied(AUDIT_ANOM_LINK, "linkat");
	return -EPERM;
}