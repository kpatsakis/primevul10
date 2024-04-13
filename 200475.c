int file_permission(struct file *file, int mask)
{
	return permission(file->f_path.dentry->d_inode, mask, NULL);
}