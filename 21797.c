http_ForceField(struct http *to, unsigned n, const char *t)
{
	int i;

	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);
	assert(n < HTTP_HDR_FIRST);
	assert(n == HTTP_HDR_METHOD || n == HTTP_HDR_PROTO);
	AN(t);

	/* NB: method names and protocol versions are case-sensitive. */
	if (to->hd[n].b == NULL || strcmp(to->hd[n].b, t)) {
		i = (HTTP_HDR_UNSET - HTTP_HDR_METHOD);
		i += to->logtag;
		/* XXX: this is a dead branch */
		if (n >= HTTP_HDR_FIRST)
			VSLbt(to->vsl, (enum VSL_tag_e)i, to->hd[n]);
		http_SetH(to, n, t);
	}
}