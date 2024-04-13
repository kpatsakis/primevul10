static int str_in_list(const char *str, char * const list[], int count)
{
	int i;

	for (i = 0; i < count; i++)
		if (!strcmp(list[i], str))
			return 1;

	return 0;
}