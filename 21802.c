http_PutResponse(struct http *to, const char *proto, uint16_t status,
    const char *reason)
{

	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);
	if (proto != NULL)
		http_SetH(to, HTTP_HDR_PROTO, proto);
	http_SetStatus(to, status, reason);
}