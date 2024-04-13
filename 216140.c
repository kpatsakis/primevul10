R_API const char *r_str_casestr(const char *a, const char *b) {
	// That's a GNUism that works in many places.. but we don't want it
	// return strcasestr (a, b);
	size_t hay_len = strlen (a);
	size_t needle_len = strlen (b);
	if (!hay_len || !needle_len) {
		return NULL;
	}
	while (hay_len >= needle_len) {
		if (!r_str_ncasecmp (a, b, needle_len)) {
			return (const char *) a;
		}
		a++;
		hay_len--;
	}
	return NULL;
}