static ext4_fsblk_t ext4_find_goal(struct inode *inode, ext4_lblk_t block,
		Indirect *partial)
{
	/*
	 * XXX need to get goal block from mballoc's data structures
	 */

	return ext4_find_near(inode, partial);
}