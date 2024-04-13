static void setup_locale(void)
{
	const char *s;

	if (setlocale(LC_CTYPE, "en_US.UTF-8") == NULL) {
		if (setlocale(LC_CTYPE, "") == NULL)
			tmate_fatal("invalid LC_ALL, LC_CTYPE or LANG");
		s = nl_langinfo(CODESET);
		if (strcasecmp(s, "UTF-8") != 0 &&
		    strcasecmp(s, "UTF8") != 0)
			tmate_fatal("need UTF-8 locale (LC_CTYPE) but have %s", s);
	}

	setlocale(LC_TIME, "");
	tzset();
}