break_list(char *list,
		 char *broken_list[MAX_ELEMENTS], int *size)
{
	char *p = list;

	*size = 0;

	do {
		broken_list[*size] = p;

		(*size)++;

		p = strchr(p, ':');
		if (p) {
			*p = 0;
			p++;	/* move to next entry and skip white
				 * space.
				 */
			while (*p == ' ')
				p++;
		}
	}
	while (p != NULL && *size < MAX_ELEMENTS);
}