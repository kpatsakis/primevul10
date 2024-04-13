http_PutField(struct http *to, int field, const char *string)
{
	const char *p;

	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);
	p = WS_Copy(to->ws, string, -1);
	if (p == NULL) {
		http_fail(to);
		VSLbs(to->vsl, SLT_LostHeader, TOSTRAND(string));
		return;
	}
	http_SetH(to, field, p);
}