SDB_API bool sdb_disk_unlink (Sdb *s) {
	return (s->dir && *(s->dir) && unlink (s->dir) != -1);
}