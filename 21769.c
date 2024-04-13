HTTP_GetStatusPack(struct worker *wrk, struct objcore *oc)
{
	const char *ptr;
	ptr = ObjGetAttr(wrk, oc, OA_HEADERS, NULL);
	AN(ptr);

	return (vbe16dec(ptr + 2));
}