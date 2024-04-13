R_API wchar_t* r_str_mb_to_wc(const char *buf) {
	if (!buf) {
		return NULL;
	}
	return r_str_mb_to_wc_l (buf, strlen (buf));
}