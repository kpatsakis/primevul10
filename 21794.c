http_EstimateWS(const struct http *fm, unsigned how)
{
	unsigned u, l;

	l = 4;
	CHECK_OBJ_NOTNULL(fm, HTTP_MAGIC);
	for (u = 0; u < fm->nhd; u++) {
		if (u == HTTP_HDR_METHOD || u == HTTP_HDR_URL)
			continue;
		Tcheck(fm->hd[u]);
		if (http_isfiltered(fm, u, how))
			continue;
		l += Tlen(fm->hd[u]) + 1L;
	}
	return (PRNDUP(l + 1L));
}