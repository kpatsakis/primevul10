static void udf_put_super(struct super_block *sb)
{
	struct udf_sb_info *sbi;

	sbi = UDF_SB(sb);

	if (sbi->s_vat_inode)
		iput(sbi->s_vat_inode);
#ifdef CONFIG_UDF_NLS
	if (UDF_QUERY_FLAG(sb, UDF_FLAG_NLS_MAP))
		unload_nls(sbi->s_nls_map);
#endif
	if (!(sb->s_flags & MS_RDONLY))
		udf_close_lvid(sb);
	brelse(sbi->s_lvid_bh);
	udf_sb_free_partitions(sb);
	kfree(sb->s_fs_info);
	sb->s_fs_info = NULL;
}