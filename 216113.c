R_API char *r_str_ndup(const char *ptr, int len) {
	if (len < 0) {
		return NULL;
	}
	char *out = malloc (len + 1);
	if (!out) {
		return NULL;
	}
	strncpy (out, ptr, len);
	out[len] = 0;
	return out;
}