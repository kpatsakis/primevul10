R_API wchar_t* r_str_mb_to_wc_l(const char *buf, int len) {
	wchar_t *res_buf = NULL;
	size_t sz;
	bool fail = true;

	if (!buf || len <= 0) {
		return NULL;
	}
	sz = mbstowcs (NULL, buf, len);
	if (sz == (size_t)-1) {
		goto err_r_str_mb_to_wc;
	}
	res_buf = (wchar_t *)calloc (1, (sz + 1) * sizeof (wchar_t));
	if (!res_buf) {
		goto err_r_str_mb_to_wc;
	}
	sz = mbstowcs (res_buf, buf, sz + 1);
	if (sz == (size_t)-1) {
		goto err_r_str_mb_to_wc;
	}
	fail = false;
err_r_str_mb_to_wc:
	if (fail) {
		R_FREE (res_buf);
	}
	return res_buf;
}