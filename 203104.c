theme_adium_match_with_format (const gchar **str,
			       const gchar *match,
			       gchar **format)
{
	const gchar *cur = *str;
	const gchar *end;

	if (!theme_adium_match (&cur, match)) {
		return FALSE;
	}
	cur++;

	end = strstr (cur, "}%");
	if (!end) {
		return FALSE;
	}

	*format = g_strndup (cur , end - cur);
	*str = end + 1;
	return TRUE;
}