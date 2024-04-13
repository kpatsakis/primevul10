R_API char *r_core_anal_get_comments(RCore *core, ut64 addr) {
	if (core) {
		char *type = r_meta_get_string (core->anal, R_META_TYPE_VARTYPE, addr);
		char *cmt = r_meta_get_string (core->anal, R_META_TYPE_COMMENT, addr);
		if (type && cmt) {
			char *ret = r_str_newf ("%s %s", type, cmt);
			free (type);
			free (cmt);
			return ret;
		} else if (type) {
			return type;
		} else if (cmt) {
			return cmt;
		}
	}
	return NULL;
}