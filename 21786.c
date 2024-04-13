http_GetContentRange(const struct http *hp, ssize_t *lo, ssize_t *hi)
{
	ssize_t tmp, cl;
	const char *b, *t;

	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);

	if (lo == NULL)
		lo = &tmp;
	if (hi == NULL)
		hi = &tmp;

	*lo = *hi = -1;

	if (!http_GetHdr(hp, H_Content_Range, &b))
		return (-1);

	t = strchr(b, ' ');
	if (t == NULL)
		return (-2);		// Missing space after range unit

	if (!http_range_at(b, bytes, t - b))
		return (-1);		// Unknown range unit, ignore
	b = t + 1;

	if (*b == '*') {		// Content-Range: bytes */123
		*lo = *hi = -1;
		b++;
	} else {			// Content-Range: bytes 1-2/3
		*lo = VNUM_uint(b, NULL, &b);
		if (*lo < 0)
			return (-2);
		if (*b != '-')
			return (-2);
		*hi = VNUM_uint(b + 1, NULL, &b);
		if (*hi < 0)
			return (-2);
	}
	if (*b != '/')
		return (-2);
	if (b[1] == '*') {		// Content-Range: bytes 1-2/*
		cl = -1;
		b += 2;
	} else {
		cl = VNUM_uint(b + 1, NULL, &b);
		if (cl <= 0)
			return (-2);
	}
	while (vct_islws(*b))
		b++;
	if (*b != '\0')
		return (-2);
	if (*lo > *hi)
		return (-2);
	assert(cl >= -1);
	if (*lo >= cl || *hi >= cl)
		return (-2);
	AN(cl);
	return (cl);
}