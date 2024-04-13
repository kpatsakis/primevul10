R_API char *r_str_utf16_decode(const ut8 *s, int len) {
	int i = 0;
	int j = 0;
	char *result = NULL;
	int count_unicode = 0;
	int count_ascii = 0;
	int lenresult = 0;
	if (!s) {
		return NULL;
	}
	for (i = 0; i < len && (s[i] || s[i+1]); i += 2) {
		if (!s[i+1] && 0x20 <= s[i] && s[i] <= 0x7E) {
			++count_ascii;
		} else {
			++count_unicode;
		}
	}
	lenresult = 1 + count_ascii + count_unicode * 6; // len("\\uXXXX") = 6
	if (!(result = calloc (1 + count_ascii + count_unicode * 6, 1))) {
		return NULL;
	}
	for (i = 0; i < len && j < lenresult && (s[i] || s[i+1]); i += 2) {
		if (!s[i+1] && IS_PRINTABLE(s[i])) {
			result[j++] = s[i];
		} else {
			j += snprintf (&result[j], lenresult - j, "\\u%.2"HHXFMT"%.2"HHXFMT"", s[i], s[i+1]);
		}
	}
	return result;
}