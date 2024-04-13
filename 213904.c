remove_bad_chars(char *s)
{
	while (*s) {
		if ((*s == '(') || (*s == ')') || (*s == ' '))
			*s = '_';
		++s;
	}
}