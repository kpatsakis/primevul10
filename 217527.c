static int __init setup_slub_min_order(char *str)
{
	get_option(&str, (int *)&slub_min_order);

	return 1;
}