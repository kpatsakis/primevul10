static int prompt_flag (RCore *r, char *s, size_t maxlen) {
	const char DOTS[] = "...";
	const RFlagItem *f = r_flag_get_at (r->flags, r->offset, false);
	if (!f) {
		return false;
	}
	if (f->offset < r->offset) {
		snprintf (s, maxlen, "%s + %" PFMT64u, f->name, r->offset - f->offset);
	} else {
		snprintf (s, maxlen, "%s", f->name);
	}
	if (strlen (s) > maxlen - sizeof (DOTS)) {
		s[maxlen - sizeof (DOTS) - 1] = '\0';
		strcat (s, DOTS);
	}
	return true;
}