void ext4_get_inode_flags(struct ext4_inode_info *ei)
{
	unsigned int flags = ei->vfs_inode.i_flags;

	ei->i_flags &= ~(EXT4_SYNC_FL|EXT4_APPEND_FL|
			EXT4_IMMUTABLE_FL|EXT4_NOATIME_FL|EXT4_DIRSYNC_FL);
	if (flags & S_SYNC)
		ei->i_flags |= EXT4_SYNC_FL;
	if (flags & S_APPEND)
		ei->i_flags |= EXT4_APPEND_FL;
	if (flags & S_IMMUTABLE)
		ei->i_flags |= EXT4_IMMUTABLE_FL;
	if (flags & S_NOATIME)
		ei->i_flags |= EXT4_NOATIME_FL;
	if (flags & S_DIRSYNC)
		ei->i_flags |= EXT4_DIRSYNC_FL;
}