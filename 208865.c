void check_option(const char *opt)
{
	int i;
	for (i = 0; invalid_opts[i] != NULL; i++)
		if (strcmp(opt, invalid_opts[i]) == 0)
			errx(1, "%s: not allowed option", opt);
}