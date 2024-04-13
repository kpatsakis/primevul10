http_SetHeader(struct http *to, const char *header)
{

	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);
	if (to->nhd >= to->shd) {
		VSLbs(to->vsl, SLT_LostHeader, TOSTRAND(header));
		http_fail(to);
		return;
	}
	http_SetH(to, to->nhd++, header);
}