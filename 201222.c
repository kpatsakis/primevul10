
static inline int ext4_is_group_locked(struct super_block *sb,
					ext4_group_t group)
{
	struct ext4_group_info *grinfo = ext4_get_group_info(sb, group);

	return bit_spin_is_locked(EXT4_GROUP_INFO_LOCKED_BIT,
						&(grinfo->bb_state));