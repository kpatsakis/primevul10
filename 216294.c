R_API size_t r_str_utf8_codepoint(const char* s, size_t left) {
	if ((*s & 0x80) != 0x80) {
		return 0;
	} else if ((*s & 0xe0) == 0xc0 && left >= 1) {
		return ((*s & 0x1f) << 6) + (*(s + 1) & 0x3f);
	} else if ((*s & 0xf0) == 0xe0 && left >= 2) {
		return ((*s & 0xf) << 12) + ((*(s + 1) & 0x3f) << 6) + (*(s + 2) & 0x3f);
	} else if ((*s & 0xf8) == 0xf0 && left >= 3) {
		return ((*s & 0x7) << 18) + ((*(s + 1) & 0x3f) << 12) + ((*(s + 2) & 0x3f) << 6) + (*(s + 3) & 0x3f);
	}
	return 0;
}