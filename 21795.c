http_ForceHeader(struct http *to, hdr_t hdr, const char *val)
{

	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);
	if (http_HdrIs(to, hdr, val))
		return;
	http_Unset(to, hdr);
	http_PrintfHeader(to, "%s %s", hdr + 1, val);
}