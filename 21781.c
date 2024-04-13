http_GetStatus(const struct http *hp)
{

	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);
	return (hp->status);
}