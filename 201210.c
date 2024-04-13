
static inline void ext4_update_i_disksize(struct inode *inode, loff_t newsize)
{
	/*
	 * XXX: replace with spinlock if seen contended -bzzz
	 */
	down_write(&EXT4_I(inode)->i_data_sem);
	if (newsize > EXT4_I(inode)->i_disksize)
		EXT4_I(inode)->i_disksize = newsize;
	up_write(&EXT4_I(inode)->i_data_sem);
	return ;