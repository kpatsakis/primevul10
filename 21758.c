http_HdrIs(const struct http *hp, hdr_t hdr, const char *val)
{
	const char *p;

	if (!http_GetHdr(hp, hdr, &p))
		return (0);
	AN(p);
	if (http_tok_eq(p, val))
		return (1);
	return (0);
}