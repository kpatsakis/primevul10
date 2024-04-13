read_next_token(char **cpp)
{
	register char *cp = *cpp;
	char *start;

	if (cp == (char *)0) {
		*cpp = (char *)0;
		return ((char *)0);
	}
	while (*cp == ' ' || *cp == '\t')
		cp++;
	if (*cp == '\0') {
		*cpp = (char *)0;
		return ((char *)0);
	}
	start = cp;
	while (*cp && *cp != ' ' && *cp != '\t')
		cp++;
	if (*cp != '\0')
		*cp++ = '\0';
	*cpp = cp;
	return (start);
}