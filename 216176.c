R_API char *r_str_escape_utf8_keep_printable(const char *buf, bool show_asciidot, bool esc_bslash) {
	return r_str_escape_utf (buf, -1, R_STRING_ENC_UTF8, show_asciidot, esc_bslash, true);
}