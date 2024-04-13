http_CountHdr(const struct http *hp, hdr_t hdr)
{
	unsigned retval = 0;
	unsigned u;

	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);

	for (u = HTTP_HDR_FIRST; u < hp->nhd; u++) {
		Tcheck(hp->hd[u]);
		if (http_IsHdr(&hp->hd[u], hdr))
			retval++;
	}
	return (retval);
}