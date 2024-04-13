http_CopyHome(const struct http *hp)
{
	unsigned u, l;
	const char *p;

	for (u = 0; u < hp->nhd; u++) {
		if (hp->hd[u].b == NULL) {
			assert(u < HTTP_HDR_FIRST);
			continue;
		}

		l = Tlen(hp->hd[u]);
		if (WS_Allocated(hp->ws, hp->hd[u].b, l))
			continue;

		p = WS_Copy(hp->ws, hp->hd[u].b, l + 1L);
		if (p == NULL) {
			http_fail(hp);
			VSLbs(hp->vsl, SLT_LostHeader, TOSTRAND(hp->hd[u].b));
			return;
		}
		hp->hd[u].b = p;
		hp->hd[u].e = p + l;
	}
}