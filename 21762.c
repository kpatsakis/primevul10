http_IsStatus(const struct http *hp, int val)
{

	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);
	assert(val >= 100 && val <= 999);
	return (val == (hp->status % 1000));
}