void ext4_set_inode_flags(struct inode *inode)
{
	unsigned int flags = EXT4_I(inode)->i_flags;

	inode->i_flags &= ~(S_SYNC|S_APPEND|S_IMMUTABLE|S_NOATIME|S_DIRSYNC);
	if (flags & EXT4_SYNC_FL)
		inode->i_flags |= S_SYNC;
	if (flags & EXT4_APPEND_FL)
		inode->i_flags |= S_APPEND;
	if (flags & EXT4_IMMUTABLE_FL)
		inode->i_flags |= S_IMMUTABLE;
	if (flags & EXT4_NOATIME_FL)
		inode->i_flags |= S_NOATIME;
	if (flags & EXT4_DIRSYNC_FL)
		inode->i_flags |= S_DIRSYNC;
}