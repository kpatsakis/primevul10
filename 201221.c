
static inline void ext4_unlock_group(struct super_block *sb,
					ext4_group_t group)
{
	struct ext4_group_info *grinfo = ext4_get_group_info(sb, group);

	bit_spin_unlock(EXT4_GROUP_INFO_LOCKED_BIT, &(grinfo->bb_state));