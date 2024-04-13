SDB_API bool sdb_disk_finish (Sdb* s) {
	bool reopen = false, ret = true;
	IFRET (!cdb_make_finish (&s->m));
#if USE_MMAN
	IFRET (fsync (s->fdump));
#endif
	IFRET (close (s->fdump));
	s->fdump = -1;
	// close current fd to avoid sharing violations
	if (s->fd != -1) {
		close (s->fd);
		s->fd = -1;
		reopen = true;
	}
#if __SDB_WINDOWS__
	LPTSTR ndump_ = r_sys_conv_utf8_to_utf16 (s->ndump);
	LPTSTR dir_ = r_sys_conv_utf8_to_utf16 (s->dir);

	if (MoveFileEx (ndump_, dir_, MOVEFILE_REPLACE_EXISTING)) {
		//eprintf ("Error 0x%02x\n", GetLastError ());
	}
	free (ndump_);
	free (dir_);
#else
	if (s->ndump && s->dir) {
		IFRET (rename (s->ndump, s->dir));
	}
#endif
	free (s->ndump);
	s->ndump = NULL;
	// reopen if was open before
	reopen = true; // always reopen if possible
	if (reopen) {
		int rr = sdb_open (s, s->dir);
		if (ret && rr < 0) {
			ret = false;
		}
		cdb_init (&s->db, s->fd);
	}
	return ret;
}