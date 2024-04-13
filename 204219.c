find_locale (void)
{
	const char *encoding = nl_langinfo(CODESET);

	if (!encoding || !*encoding)
		return xstrdup("ASCII");

   return xstrdup(encoding);
}