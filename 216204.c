R_API char *r_str_repeat(const char *ch, int sz) {
	int i;
	if (sz < 0) {
		sz = 0;
	}
	if (sz == 0) {
		return strdup ("");
	}
	RStrBuf *buf = r_strbuf_new (ch);
	for (i = 1; i < sz; i++) {
		r_strbuf_append (buf, ch);
	}
	return r_strbuf_drain (buf);
}