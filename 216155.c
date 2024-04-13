SDB_API bool sdb_disk_create(Sdb* s) {
	int nlen;
	char *str;
	const char *dir;
	if (!s || s->fdump >= 0) {
		return false; // cannot re-create
	}
	if (!s->dir && s->name) {
		s->dir = strdup (s->name);
	}
	dir = s->dir ? s->dir : "./";
	R_FREE (s->ndump);
	nlen = strlen (dir);
	str = malloc (nlen + 5);
	if (!str) {
		return false;
	}
	memcpy (str, dir, nlen + 1);
	r_sys_mkdirp (str);
	memcpy (str + nlen, ".tmp", 5);
	if (s->fdump != -1) {
		close (s->fdump);
	}
#if __SDB_WINDOWS__ && UNICODE
	wchar_t *wstr = r_sys_conv_utf8_to_utf16 (str);
	if (wstr) {
		s->fdump = _wopen (wstr, O_BINARY | O_RDWR | O_CREAT | O_TRUNC, SDB_MODE);
		free (wstr);
	} else {
		s->fdump = -1;
	}
#else
	s->fdump = open (str, O_BINARY | O_RDWR | O_CREAT | O_TRUNC, SDB_MODE);
#endif
	if (s->fdump == -1) {
		eprintf ("sdb: Cannot open '%s' for writing.\n", str);
		free (str);
		return false;
	}
	cdb_make_start (&s->m, s->fdump);
	s->ndump = str;
	return true;
}