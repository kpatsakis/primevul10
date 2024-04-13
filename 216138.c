R_API char *r_str_escape_dot(const char *buf) {
	return r_str_escape_ (buf, true, true, true, false, true);
}