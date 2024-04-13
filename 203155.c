_gdata_service_fix_uri_scheme (const gchar *uri)
{
	g_return_val_if_fail (uri != NULL && *uri != '\0', NULL);

	/* Ensure we're using the correct scheme (HTTP or HTTPS) */
	if (g_str_has_prefix (uri, "https") == FALSE) {
		gchar *fixed_uri, **pieces;

		pieces = g_strsplit (uri, ":", 2);
		g_assert (pieces[0] != NULL && pieces[1] != NULL && pieces[2] == NULL);

		fixed_uri = g_strconcat ("https:", pieces[1], NULL);

		g_strfreev (pieces);

		return fixed_uri;
	}

	return g_strdup (uri);
}