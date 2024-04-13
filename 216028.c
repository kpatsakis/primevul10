SDB_API int sdb_disk_insert(Sdb* s, const char *key, const char *val) {
	struct cdb_make *c = &s->m;
	if (!key || !val) {
		return 0;
	}
	//if (!*val) return 0; //undefine variable if no value
	return cdb_make_add (c, key, strlen (key), val, strlen (val));
}