R_API char *r_str_append(char *ptr, const char *string) {
	if (string && !ptr) {
		return strdup (string);
	}
	if (!string) {
		return ptr;
	}
	int plen = strlen (ptr);
	int slen = strlen (string);
	char *newptr = realloc (ptr, slen + plen + 1);
	if (!newptr) {
		free (ptr);
		return NULL;
	}
	ptr = newptr;
	memcpy (ptr + plen, string, slen + 1);
	return ptr;
}