R_API char *r_str_escape(const char *buf) {
	return r_str_escape_ (buf, false, true, true, false, true);
}