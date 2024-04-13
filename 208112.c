pam_conf_strnchr(char *sp, int c, intptr_t count)
{
	while (count) {
		if (*sp == (char)c)
			return ((char *)sp);
		else {
			sp++;
			count--;
		}
	};
	return (NULL);
}