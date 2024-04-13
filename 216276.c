R_API char *r_str_from_ut64(ut64 val) {
	int i = 0;
	char *v = (char *)&val;
	char *str = (char *)calloc(1, 9);
	if (!str) {
		return NULL;
	}
	while (i < 8 && *v) {
		str[i++] = *v++;
	}
	return str;
}