static wchar_t *r_utf8_to_utf16_l (const char *cstring, int len) {
	if (!cstring || !len || len < -1) {
		return NULL;
	}
	wchar_t *rutf16 = NULL;
	int wcsize;

	if ((wcsize = MultiByteToWideChar (CP_UTF8, 0, cstring, len, NULL, 0))) {
		wcsize += 1;
		if ((rutf16 = (wchar_t *) calloc (wcsize, sizeof (wchar_t)))) {
			MultiByteToWideChar (CP_UTF8, 0, cstring, len, rutf16, wcsize);
			if (len != -1) {
				rutf16[wcsize - 1] = L'\0';
			}
		}
	}
	return rutf16;
}