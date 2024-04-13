static const char *ltrim(const char *s)
{
	while (isspace(*s))
		s++;

	return s;
}