http_AppendHeader(struct http *to, hdr_t hdr, const char *val)
{
	const char *old;

	http_CollectHdr(to, hdr);
	if (http_GetHdr(to, hdr, &old)) {
		http_Unset(to, hdr);
		http_PrintfHeader(to, "%s %s, %s", hdr + 1, old, val);
	} else {
		http_PrintfHeader(to, "%s %s", hdr + 1, val);
	}
}