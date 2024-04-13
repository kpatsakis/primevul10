http_GetRange(const struct http *hp, ssize_t *lo, ssize_t *hi)
{
	ssize_t tmp;
	const char *b, *t;

	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);

	if (lo == NULL)
		lo = &tmp;
	if (hi == NULL)
		hi = &tmp;

	*lo = *hi = -1;

	if (!http_GetHdr(hp, H_Range, &b))
		return (NULL);

	t = strchr(b, '=');
	if (t == NULL)
		return ("Missing '='");

	if (!http_range_at(b, bytes, t - b))
		return ("Not Bytes");
	b = t + 1;

	*lo = VNUM_uint(b, NULL, &b);
	if (*lo == -2)
		return ("Low number too big");
	if (*b++ != '-')
		return ("Missing hyphen");

	*hi = VNUM_uint(b, NULL, &b);
	if (*hi == -2)
		return ("High number too big");
	if (*lo == -1 && *hi == -1)
		return ("Neither high nor low");
	if (*lo == -1 && *hi == 0)
		return ("No low, high is zero");
	if (*hi >= 0 && *hi < *lo)
		return ("high smaller than low");

	while (vct_islws(*b))
		b++;
	if (*b != '\0')
		return ("Trailing stuff");
	return (NULL);
}