struct dentry *lock_rename(struct dentry *p1, struct dentry *p2)
{
	struct dentry *p;

	if (p1 == p2) {
		mutex_lock_nested(&p1->d_inode->i_mutex, I_MUTEX_PARENT);
		return NULL;
	}

	mutex_lock(&p1->d_inode->i_sb->s_vfs_rename_mutex);

	for (p = p1; p->d_parent != p; p = p->d_parent) {
		if (p->d_parent == p2) {
			mutex_lock_nested(&p2->d_inode->i_mutex, I_MUTEX_PARENT);
			mutex_lock_nested(&p1->d_inode->i_mutex, I_MUTEX_CHILD);
			return p;
		}
	}

	for (p = p2; p->d_parent != p; p = p->d_parent) {
		if (p->d_parent == p1) {
			mutex_lock_nested(&p1->d_inode->i_mutex, I_MUTEX_PARENT);
			mutex_lock_nested(&p2->d_inode->i_mutex, I_MUTEX_CHILD);
			return p;
		}
	}

	mutex_lock_nested(&p1->d_inode->i_mutex, I_MUTEX_PARENT);
	mutex_lock_nested(&p2->d_inode->i_mutex, I_MUTEX_CHILD);
	return NULL;
}