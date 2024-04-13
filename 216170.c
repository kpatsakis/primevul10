R_API char *r_str_replace_in(char *str, ut32 sz, const char *key, const char *val, int g) {
	if (!str || !key || !val) {
		return NULL;
	}
	char *heaped = r_str_replace (strdup (str), key, val, g);
	if (heaped) {
		strncpy (str, heaped, sz);
		free (heaped);
	}
	return str;
}