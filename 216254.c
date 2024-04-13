R_API char *r_str_escape_utf32be(const char *buf, int buf_size, bool show_asciidot, bool esc_bslash) {
	return r_str_escape_utf (buf, buf_size, R_STRING_ENC_UTF32BE, show_asciidot, esc_bslash, false);
}