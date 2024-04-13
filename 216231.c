R_API char *r_str_prepend(char *ptr, const char *string) {
	int slen, plen;
	if (!ptr) {
		return strdup (string);
	}
	plen = strlen (ptr);
	slen = strlen (string);
	ptr = realloc (ptr, slen + plen + 1);
	if (!ptr) {
		return NULL;
	}
	memmove (ptr + slen, ptr, plen + 1);
	memmove (ptr, string, slen);
	return ptr;
}