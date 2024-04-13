static inline
struct ext4_group_info *ext4_get_group_info(struct super_block *sb,
					    ext4_group_t group)
{
	 struct ext4_group_info ***grp_info;
	 long indexv, indexh;
	 grp_info = EXT4_SB(sb)->s_group_info;
	 indexv = group >> (EXT4_DESC_PER_BLOCK_BITS(sb));
	 indexh = group & ((EXT4_DESC_PER_BLOCK(sb)) - 1);
	 return grp_info[indexv][indexh];