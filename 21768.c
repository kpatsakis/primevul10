HTTP_IterHdrPack(struct worker *wrk, struct objcore *oc, const char **p)
{
	const char *ptr;

	CHECK_OBJ_NOTNULL(wrk, WORKER_MAGIC);
	CHECK_OBJ_NOTNULL(oc, OBJCORE_MAGIC);
	AN(p);

	if (*p == NULL) {
		ptr = ObjGetAttr(wrk, oc, OA_HEADERS, NULL);
		AN(ptr);
		ptr += 4;	/* Skip nhd and status */
		ptr = strchr(ptr, '\0') + 1;	/* Skip :proto: */
		ptr = strchr(ptr, '\0') + 1;	/* Skip :status: */
		ptr = strchr(ptr, '\0') + 1;	/* Skip :reason: */
		*p = ptr;
	} else {
		*p = strchr(*p, '\0') + 1;	/* Skip to next header */
	}
	if (**p == '\0')
		return (0);
	return (1);
}