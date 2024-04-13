static int __init setup_slub_memcg_sysfs(char *str)
{
	int v;

	if (get_option(&str, &v) > 0)
		memcg_sysfs_enabled = v;

	return 1;
}