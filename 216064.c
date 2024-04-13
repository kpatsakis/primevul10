R_API char *r_str_escape_latin1(const char *buf, bool show_asciidot, bool esc_bslash, bool colors) {
	return r_str_escape_ (buf, false, colors, !colors, show_asciidot, esc_bslash);
}