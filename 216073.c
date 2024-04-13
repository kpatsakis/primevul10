R_API const char *r_str_str_xy(const char *s, const char *word, const char *prev, int *x, int *y) {
	r_return_val_if_fail (s && word && x && y, NULL);
	r_return_val_if_fail (word[0] != '\0' && word[0] != '\n', NULL);
	const char *src = prev ? prev + 1 : s;
	const char *d = strstr (src, word);
	if (!d) {
		return NULL;
	}
	const char *q;
	for (q = prev ? prev : s; q < d; q++) {
		if (*q == '\n') {
			(*y)++;
			*x = 0;

		} else {
			(*x)++;
		}
	}
	return d;
}