http_PrintfHeader(struct http *to, const char *fmt, ...)
{
	va_list ap, ap2;
	struct vsb vsb[1];
	size_t sz;
	char *p;

	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);

	va_start(ap, fmt);
	va_copy(ap2, ap);

	WS_VSB_new(vsb, to->ws);
	VSB_vprintf(vsb, fmt, ap);
	p = WS_VSB_finish(vsb, to->ws, &sz);

	if (p == NULL || to->nhd >= to->shd) {
		http_fail(to);
		VSLbv(to->vsl, SLT_LostHeader, fmt, ap2);
	} else {
		http_SetH(to, to->nhd++, p);
	}
	va_end(ap);
	va_end(ap2);
}