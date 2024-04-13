HTTP_Encode(const struct http *fm, uint8_t *p0, unsigned l, unsigned how)
{
	unsigned u, w;
	uint16_t n;
	uint8_t *p, *e;

	AN(p0);
	AN(l);
	p = p0;
	e = p + l;
	assert(p + 5 <= e);
	assert(fm->nhd <= fm->shd);
	n = HTTP_HDR_FIRST - 3;
	vbe16enc(p + 2, fm->status);
	p += 4;
	CHECK_OBJ_NOTNULL(fm, HTTP_MAGIC);
	for (u = 0; u < fm->nhd; u++) {
		if (u == HTTP_HDR_METHOD || u == HTTP_HDR_URL)
			continue;
		Tcheck(fm->hd[u]);
		if (http_isfiltered(fm, u, how))
			continue;
		http_VSLH(fm, u);
		w = Tlen(fm->hd[u]) + 1L;
		assert(p + w + 1 <= e);
		memcpy(p, fm->hd[u].b, w);
		p += w;
		n++;
	}
	*p++ = '\0';
	assert(p <= e);
	vbe16enc(p0, n + 1);
}