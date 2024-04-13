static int print_ref_info(const char *refname, const struct object_id *oid,
                          int flags, void *cb_data)
{
	struct object *obj;

	if (!(obj = parse_object(oid)))
		return 0;

	htmlf("%s\t%s\n", oid_to_hex(oid), refname);
	if (obj->type == OBJ_TAG) {
		if (!(obj = deref_tag(obj, refname, 0)))
			return 0;
		htmlf("%s\t%s^{}\n", oid_to_hex(&obj->oid), refname);
	}
	return 0;
}