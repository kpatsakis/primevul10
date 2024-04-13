http_GetContentLength(const struct http *hp)
{
	ssize_t cl;
	const char *b;

	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);

	if (!http_GetHdr(hp, H_Content_Length, &b))
		return (-1);
	cl = VNUM_uint(b, NULL, &b);
	if (cl < 0)
		return (-2);
	while (vct_islws(*b))
		b++;
	if (*b != '\0')
		return (-2);
	return (cl);
}