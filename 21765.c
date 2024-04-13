http_IsHdr(const txt *hh, hdr_t hdr)
{
	unsigned l;

	Tcheck(*hh);
	AN(hdr);
	l = hdr[0];
	assert(l == strlen(hdr + 1));
	assert(hdr[l] == ':');
	hdr++;
	return (http_hdr_at(hdr, hh->b, l));
}