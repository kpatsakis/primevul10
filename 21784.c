http_Teardown(struct http *hp)
{

	CHECK_OBJ_NOTNULL(hp, HTTP_MAGIC);
	AN(hp->shd);
	memset(&hp->nhd, 0, sizeof *hp - offsetof(struct http, nhd));
	memset(hp->hd, 0, sizeof *hp->hd * hp->shd);
	memset(hp->hdf, 0, sizeof *hp->hdf * hp->shd);
}