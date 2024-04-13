R_API char* r_str_wc_to_mb_l(const wchar_t *buf, int len) {
	char *res_buf = NULL;
	bool fail = true;
	size_t sz;

	if (!buf || len <= 0) {
		return NULL;
	}
	sz = wcstombs (NULL, buf, len);
	if (sz == (size_t)-1) {
		goto err_r_str_wc_to_mb;
	}
	res_buf = (char *)calloc (1, (sz + 1) * sizeof (char));
	if (!res_buf) {
		goto err_r_str_wc_to_mb;
	}
	sz = wcstombs (res_buf, buf, sz + 1);
	if (sz == (size_t)-1) {
		goto err_r_str_wc_to_mb;
	}
	fail = false;
err_r_str_wc_to_mb:
	if (fail) {
		R_FREE (res_buf);
	}
	return res_buf;
}