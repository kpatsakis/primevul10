HTTP_Decode(struct http *to, const uint8_t *fm)
{

	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);
	AN(to->vsl);
	AN(fm);
	if (vbe16dec(fm) <= to->shd) {
		to->status = vbe16dec(fm + 2);
		fm += 4;
		for (to->nhd = 0; to->nhd < to->shd; to->nhd++) {
			if (to->nhd == HTTP_HDR_METHOD ||
			    to->nhd == HTTP_HDR_URL) {
				to->hd[to->nhd].b = NULL;
				to->hd[to->nhd].e = NULL;
				continue;
			}
			if (*fm == '\0')
				return (0);
			to->hd[to->nhd].b = (const void*)fm;
			fm = (const void*)strchr((const void*)fm, '\0');
			to->hd[to->nhd].e = (const void*)fm;
			fm++;
			http_VSLH(to, to->nhd);
		}
	}
	VSLb(to->vsl, SLT_Error,
	    "Too many headers to Decode object (%u vs. %u)",
	    vbe16dec(fm), to->shd);
	return (-1);
}