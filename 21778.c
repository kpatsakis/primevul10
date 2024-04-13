http_FilterReq(struct http *to, const struct http *fm, unsigned how)
{
	unsigned u;

	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);
	CHECK_OBJ_NOTNULL(fm, HTTP_MAGIC);

	http_linkh(to, fm, HTTP_HDR_METHOD);
	http_linkh(to, fm, HTTP_HDR_URL);
	http_linkh(to, fm, HTTP_HDR_PROTO);
	to->protover = fm->protover;
	to->status = fm->status;

	to->nhd = HTTP_HDR_FIRST;
	for (u = HTTP_HDR_FIRST; u < fm->nhd; u++) {
		Tcheck(fm->hd[u]);
		if (http_isfiltered(fm, u, how))
			continue;
		assert (to->nhd < to->shd);
		to->hd[to->nhd] = fm->hd[u];
		to->hdf[to->nhd] = 0;
		http_VSLH(to, to->nhd);
		to->nhd++;
	}
}