R_API char* r_str_wc_to_mb(const wchar_t *buf) {
	if (!buf) {
		return NULL;
	}
	return r_str_wc_to_mb_l (buf, wcslen (buf));
}