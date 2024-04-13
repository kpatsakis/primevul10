static int __init setup_slub_max_order(char *str)
{
	get_option(&str, (int *)&slub_max_order);
	slub_max_order = min(slub_max_order, (unsigned int)MAX_ORDER - 1);

	return 1;
}