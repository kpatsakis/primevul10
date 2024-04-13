escape_and_append_len (GString *string, const gchar *str, gint len)
{
	while (str != NULL && *str != '\0' && len != 0) {
		switch (*str) {
		case '\\':
			/* \ becomes \\ */
			g_string_append (string, "\\\\");
			break;
		case '\"':
			/* " becomes \" */
			g_string_append (string, "\\\"");
			break;
		case '\n':
			/* Remove end of lines */
			break;
		default:
			g_string_append_c (string, *str);
		}

		str++;
		len--;
	}
}