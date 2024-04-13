http_DoConnection(struct http *hp, stream_close_t sc_close)
{
	const char *h, *b, *e;
	stream_close_t retval;
	unsigned u, v;
	struct http_hdrflg *f;

	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);
	assert(sc_close == SC_REQ_CLOSE || sc_close == SC_RESP_CLOSE);

	if (hp->protover == 10)
		retval = SC_REQ_HTTP10;
	else
		retval = SC_NULL;

	http_CollectHdr(hp, H_Connection);
	if (!http_GetHdr(hp, H_Connection, &h))
		return (retval);
	AN(h);
	while (http_split(&h, NULL, ",", &b, &e)) {
		u = pdiff(b, e);
		if (u == 5 && http_hdr_at(b, "close", u))
			retval = sc_close;
		if (u == 10 && http_hdr_at(b, "keep-alive", u))
			retval = SC_NULL;

		/* Refuse removal of well-known-headers if they would pass. */
		f = http_hdr_flags(b, e);
		if (f != NULL && !(f->flag & HTTPH_R_PASS))
			return (SC_RX_BAD);

		for (v = HTTP_HDR_FIRST; v < hp->nhd; v++) {
			Tcheck(hp->hd[v]);
			if (hp->hd[v].e < hp->hd[v].b + u + 1)
				continue;
			if (hp->hd[v].b[u] != ':')
				continue;
			if (!http_hdr_at(b, hp->hd[v].b, u))
				continue;
			hp->hdf[v] |= HDF_FILTER;
		}
	}
	CHECK_OBJ_NOTNULL(retval, STREAM_CLOSE_MAGIC);
	return (retval);
}