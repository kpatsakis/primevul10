http_GetHdrToken(const struct http *hp, hdr_t hdr,
    const char *token, const char **pb, const char **pe)
{
	const char *h, *b, *e;

	if (pb != NULL)
		*pb = NULL;
	if (pe != NULL)
		*pe = NULL;
	if (!http_GetHdr(hp, hdr, &h))
		return (0);
	AN(h);

	while (http_split(&h, NULL, ",", &b, &e))
		if (http_istoken(&b, e, token))
			break;
	if (b == NULL)
		return (0);
	if (pb != NULL) {
		for (; vct_islws(*b); b++)
			continue;
		if (b == e) {
			b = NULL;
			e = NULL;
		}
		*pb = b;
		if (pe != NULL)
			*pe = e;
	}
	return (1);
}