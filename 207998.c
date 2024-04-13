xstrncpy (char *dest, const char *src, size_t n) {
	if ( (src == NULL) || (strcmp(src, "") == 0))
		return;
	strncpy(dest, src, n-1);
	dest[n-1] = 0;
}