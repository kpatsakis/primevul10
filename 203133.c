_gdata_service_build_uri (const gchar *format, ...)
{
	const gchar *p;
	gchar *fixed_uri;
	GString *uri;
	va_list args;

	g_return_val_if_fail (format != NULL, NULL);

	/* Allocate a GString to build the URI in with at least as much space as the format string */
	uri = g_string_sized_new (strlen (format));

	/* Build the URI */
	va_start (args, format);

	for (p = format; *p != '\0'; p++) {
		if (*p != '%') {
			g_string_append_c (uri, *p);
			continue;
		}

		switch(*++p) {
			case 's':
				g_string_append_uri_escaped (uri, va_arg (args, gchar*), NULL, TRUE);
				break;
			case 'p':
				g_string_append (uri, va_arg (args, gchar*));
				break;
			case 'd':
				g_string_append_printf (uri, "%d", va_arg (args, gint));
				break;
			case '%':
				g_string_append_c (uri, '%');
				break;
			default:
				g_error ("Unrecognized format placeholder '%%%c' in format '%s'. This is a programmer error.", *p, format);
				break;
		}
	}

	va_end (args);

	/* Fix the scheme to always be HTTPS */
	fixed_uri = _gdata_service_fix_uri_scheme (uri->str);
	g_string_free (uri, TRUE);

	return fixed_uri;
}