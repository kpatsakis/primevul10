http_SetH(struct http *to, unsigned n, const char *header)
{

	assert(n < to->nhd);
	AN(header);
	to->hd[n].b = TRUST_ME(header);
	to->hd[n].e = strchr(to->hd[n].b, '\0');
	to->hdf[n] = 0;
	http_VSLH(to, n);
	if (n == HTTP_HDR_PROTO)
		http_Proto(to);
}