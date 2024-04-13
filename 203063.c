theme_adium_match (const gchar **str,
		   const gchar *match)
{
	gint len;

	len = strlen (match);
	if (strncmp (*str, match, len) == 0) {
		*str += len - 1;
		return TRUE;
	}

	return FALSE;
}