http_TimeHeader(struct http *to, const char *fmt, vtim_real now)
{
	char *p;

	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);
	if (to->nhd >= to->shd) {
		VSLbs(to->vsl, SLT_LostHeader, TOSTRAND(fmt));
		http_fail(to);
		return;
	}
	p = WS_Alloc(to->ws, strlen(fmt) + VTIM_FORMAT_SIZE);
	if (p == NULL) {
		http_fail(to);
		VSLbs(to->vsl, SLT_LostHeader, TOSTRAND(fmt));
		return;
	}
	strcpy(p, fmt);
	VTIM_format(now, strchr(p, '\0'));
	http_SetH(to, to->nhd++, p);
}