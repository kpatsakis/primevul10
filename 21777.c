HTTP_Merge(struct worker *wrk, struct objcore *oc, struct http *to)
{
	const char *ptr;
	unsigned u;
	const char *p;
	unsigned nhd_before_merge;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(oc, OBJCORE_MAGIC);
	CHECK_OBJ_NOTNULL(to, HTTP_MAGIC);

	ptr = ObjGetAttr(wrk, oc, OA_HEADERS, NULL);
	AN(ptr);

	to->status = vbe16dec(ptr + 2);
	ptr += 4;

	for (u = 0; u < HTTP_HDR_FIRST; u++) {
		if (u == HTTP_HDR_METHOD || u == HTTP_HDR_URL)
			continue;
		http_SetH(to, u, ptr);
		ptr = strchr(ptr, '\0') + 1;
	}
	nhd_before_merge = to->nhd;
	while (*ptr != '\0') {
		p = strchr(ptr, ':');
		AN(p);
		u = http_findhdr(to, p - ptr, ptr);
		if (u == 0 || u >= nhd_before_merge)
			http_SetHeader(to, ptr);
		ptr = strchr(ptr, '\0') + 1;
	}
}