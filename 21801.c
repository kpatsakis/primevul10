http_split(const char **src, const char *stop, const char *sep,
    const char **b, const char **e)
{
	const char *p, *q;

	AN(src);
	AN(*src);
	AN(sep);
	AN(b);
	AN(e);

	if (stop == NULL)
		stop = strchr(*src, '\0');

	for (p = *src; p < stop && (vct_issp(*p) || strchr(sep, *p)); p++)
		continue;

	if (p >= stop) {
		*b = NULL;
		*e = NULL;
		return (0);
	}

	*b = p;
	if (*sep == '\0') {
		for (q = p + 1; q < stop && !vct_issp(*q); q++)
			continue;
		*e = q;
		*src = q;
		return (1);
	}
	for (q = p + 1; q < stop && !strchr(sep, *q); q++)
		continue;
	*src = q;
	while (q > p && vct_issp(q[-1]))
		q--;
	*e = q;
	return (1);
}