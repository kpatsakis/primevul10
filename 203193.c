static void udf_sb_free_partitions(struct super_block *sb)
{
	struct udf_sb_info *sbi = UDF_SB(sb);
	int i;

	for (i = 0; i < sbi->s_partitions; i++)
		udf_free_partition(&sbi->s_partmaps[i]);
	kfree(sbi->s_partmaps);
	sbi->s_partmaps = NULL;
}