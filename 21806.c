http_CollectHdrSep(struct http *hp, hdr_t hdr, const char *sep)
{
	unsigned u, l, lsep, ml, f, x, d;
	char *b = NULL, *e = NULL;
	const char *v;

	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);
	if (WS_Overflowed(hp->ws))
		return;

	if (sep == NULL || *sep == '\0')
		sep = ", ";
	lsep = strlen(sep);

	l = hdr[0];
	assert(l == strlen(hdr + 1));
	assert(hdr[l] == ':');
	f = http_findhdr(hp, l - 1, hdr + 1);
	if (f == 0)
		return;

	for (d = u = f + 1; u < hp->nhd; u++) {
		Tcheck(hp->hd[u]);
		if (!http_IsHdr(&hp->hd[u], hdr)) {
			if (d != u) {
				hp->hd[d] = hp->hd[u];
				hp->hdf[d] = hp->hdf[u];
			}
			d++;
			continue;
		}
		if (b == NULL) {
			/* Found second header, start our collection */
			ml = WS_ReserveAll(hp->ws);
			b = WS_Reservation(hp->ws);
			e = b + ml;
			x = Tlen(hp->hd[f]);
			if (b + x >= e) {
				http_fail(hp);
				VSLbs(hp->vsl, SLT_LostHeader,
				    TOSTRAND(hdr + 1));
				WS_Release(hp->ws, 0);
				return;
			}
			memcpy(b, hp->hd[f].b, x);
			b += x;
		}

		AN(b);
		AN(e);

		/* Append the Nth header we found */
		x = Tlen(hp->hd[u]) - l;

		v = hp->hd[u].b + *hdr;
		while (vct_issp(*v)) {
			v++;
			x--;
		}

		if (b + lsep + x >= e) {
			http_fail(hp);
			VSLbs(hp->vsl, SLT_LostHeader, TOSTRAND(hdr + 1));
			WS_Release(hp->ws, 0);
			return;
		}
		memcpy(b, sep, lsep);
		b += lsep;
		memcpy(b, v, x);
		b += x;
	}
	if (b == NULL)
		return;
	hp->nhd = (uint16_t)d;
	AN(e);
	*b = '\0';
	hp->hd[f].b = WS_Reservation(hp->ws);
	hp->hd[f].e = b;
	WS_ReleaseP(hp->ws, b + 1);
}