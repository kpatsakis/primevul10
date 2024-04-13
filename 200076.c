static int shmem_unlink(struct inode *dir, struct dentry *dentry)
{
	struct inode *inode = dentry->d_inode;

	if (inode->i_nlink > 1 && !S_ISDIR(inode->i_mode)) {
		struct shmem_sb_info *sbinfo = SHMEM_SB(inode->i_sb);
		if (sbinfo->max_inodes) {
			spin_lock(&sbinfo->stat_lock);
			sbinfo->free_inodes++;
			spin_unlock(&sbinfo->stat_lock);
		}
	}

	dir->i_size -= BOGO_DIRENT_SIZE;
	inode->i_ctime = dir->i_ctime = dir->i_mtime = CURRENT_TIME;
	drop_nlink(inode);
	dput(dentry);	/* Undo the count from "create" - this does all the work */
	return 0;
}