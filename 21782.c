http_init_hdr(char *hdr, int flg)
{
	struct http_hdrflg *f;

	hdr[0] = strlen(hdr + 1);
	f = http_hdr_flags(hdr + 1, hdr + hdr[0]);
	if (flg) {
		AN(f);
		assert(f->hdr == hdr);
		f->flag = flg;
	}
}