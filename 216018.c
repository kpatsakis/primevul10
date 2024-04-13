R_API char *r_str_list_join(RList *str, const char *sep) {
	RStrBuf *sb = r_strbuf_new ("");
	const char *p;
	while ((p = r_list_pop_head (str))) {
		if (r_strbuf_length (sb) != 0) {
			r_strbuf_append (sb, sep);
		}
		r_strbuf_append (sb, p);
	}
	return r_strbuf_drain (sb);
}