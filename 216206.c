R_API ut64 r_str_hash64(const char *s) {
	ut64 len, h = 5381;
	if (!s) {
		return 0;
	}
	for (len = strlen (s); len > 0; len--) {
		h = (h ^ (h << 5)) ^ *s++;
	}
	return h;
}