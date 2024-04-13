replace(const char *s, const char *old, const char *new)
{
	char *ret;
	int i, count = 0;
	size_t newlen = strlen(new);
	size_t oldlen = strlen(old);

	for (i = 0; s[i] != '\0'; i++) {
		if (strstr(&s[i], old) == &s[i]) {
			count++;
			i += oldlen - 1;
		}
	}
	ret = malloc(i + 1 + count * (newlen - oldlen));
	if (ret != NULL) {
		i = 0;
		while (*s) {
			if (strstr(s, old) == s) {
				strcpy(&ret[i], new);
				i += newlen;
				s += oldlen;
			} else {
				ret[i++] = *s++;
			}
		}
		ret[i] = '\0';
	}

	return ret;
}