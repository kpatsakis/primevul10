string_with_format (const gchar *format,
		    const gchar *first_string,
		    ...)
{
	va_list args;
	const gchar *str;
	GString *result;

	va_start (args, first_string);
	result = g_string_sized_new (strlen (format));
	for (str = first_string; str != NULL; str = va_arg (args, const gchar *)) {
		const gchar *next;

		next = strstr (format, "%@");
		if (next == NULL) {
			break;
		}

		g_string_append_len (result, format, next - format);
		g_string_append (result, str);
		format = next + 2;
	}
	g_string_append (result, format);
	va_end (args);

	return g_string_free (result, FALSE);
}