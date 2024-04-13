R_API bool r_str_char_fullwidth (const char* s, size_t left) {
	size_t codepoint = r_str_utf8_codepoint (s, left);
	return (codepoint >= 0x1100 &&
		 (codepoint <= 0x115f ||                  /* Hangul Jamo init. consonants */
			  codepoint == 0x2329 || codepoint == 0x232a ||
		 (R_BETWEEN (0x2e80, codepoint, 0xa4cf)
			&& codepoint != 0x303f) ||        /* CJK ... Yi */
		 R_BETWEEN (0xac00, codepoint, 0xd7a3) || /* Hangul Syllables */
		 R_BETWEEN (0xf900, codepoint, 0xfaff) || /* CJK Compatibility Ideographs */
		 R_BETWEEN (0xfe10, codepoint, 0xfe19) || /* Vertical forms */
		 R_BETWEEN (0xfe30, codepoint, 0xfe6f) || /* CJK Compatibility Forms */
		 R_BETWEEN (0xff00, codepoint, 0xff60) || /* Fullwidth Forms */
		 R_BETWEEN (0xffe0, codepoint, 0xffe6) ||
		 R_BETWEEN (0x20000, codepoint, 0x2fffd) ||
		 R_BETWEEN (0x30000, codepoint, 0x3fffd)));

}