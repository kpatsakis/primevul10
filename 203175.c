soup_log_printer (SoupLogger *logger, SoupLoggerLogLevel level, char direction, const char *data, gpointer user_data)
{
	gboolean filter_data;
	gchar *_data = NULL;

	filter_data = (_gdata_service_get_log_level () > GDATA_LOG_NONE && _gdata_service_get_log_level () < GDATA_LOG_FULL_UNREDACTED) ? TRUE : FALSE;

	if (filter_data == TRUE) {
		/* Filter out lines which look like they might contain usernames, passwords or auth. tokens. */
		if (direction == '>' && g_str_has_prefix (data, "Authorization: GoogleLogin ") == TRUE) {
			_data = g_strdup ("Authorization: GoogleLogin <redacted>");
		} else if (direction == '>' && g_str_has_prefix (data, "Authorization: OAuth ") == TRUE) {
			_data = g_strdup ("Authorization: OAuth <redacted>");
		} else if (direction == '<' && g_str_has_prefix (data, "Set-Cookie: ") == TRUE) {
			_data = g_strdup ("Set-Cookie: <redacted>");
		} else if (direction == '<' && g_str_has_prefix (data, "Location: ") == TRUE) {
			/* Looks like:
			 * "Location: https://www.google.com/calendar/feeds/default/owncalendars/full?gsessionid=sBjmp05m5i67exYA51XjDA". */
			SoupURI *uri;
			gchar *_uri;
			GHashTable *params;

			uri = soup_uri_new (data + strlen ("Location: "));

			if (uri->query != NULL) {
				params = soup_form_decode (uri->query);

				/* strdup()s are necessary because the hash table's set up to free keys. */
				if (g_hash_table_lookup (params, "gsessionid") != NULL) {
					g_hash_table_insert (params, (gpointer) g_strdup ("gsessionid"), (gpointer) "<redacted>");
				}

				soup_uri_set_query_from_form (uri, params);
				g_hash_table_destroy (params);
			}

			_uri = soup_uri_to_string (uri, FALSE);
			_data = g_strconcat ("Location: ", _uri, NULL);
			g_free (_uri);

			soup_uri_free (uri);
		} else if (direction == '<' && g_str_has_prefix (data, "SID=") == TRUE) {
			_data = g_strdup ("SID=<redacted>");
		} else if (direction == '<' && g_str_has_prefix (data, "LSID=") == TRUE) {
			_data = g_strdup ("LSID=<redacted>");
		} else if (direction == '<' && g_str_has_prefix (data, "Auth=") == TRUE) {
			_data = g_strdup ("Auth=<redacted>");
		} else if (direction == '>' && g_str_has_prefix (data, "accountType=") == TRUE) {
			/* Looks like: "> accountType=HOSTED%5FOR%5FGOOGLE&Email=[e-mail address]&Passwd=[plaintex password]"
			               "&service=[service name]&source=ytapi%2DGNOME%2Dlibgdata%2D444fubtt%2D0". */
			GHashTable *params = soup_form_decode (data);

			/* strdup()s are necessary because the hash table's set up to free keys. */
			if (g_hash_table_lookup (params, "Email") != NULL) {
				g_hash_table_insert (params, (gpointer) g_strdup ("Email"), (gpointer) "<redacted>");
			}
			if (g_hash_table_lookup (params, "Passwd") != NULL) {
				g_hash_table_insert (params, (gpointer) g_strdup ("Passwd"), (gpointer) "<redacted>");
			}

			_data = soup_form_encode_hash (params);

			g_hash_table_destroy (params);
		} else if (direction == '<' && g_str_has_prefix (data, "oauth_token=") == TRUE) {
			/* Looks like: "< oauth_token=4%2FI-WU7sBzKk5GhGlQUF8a_TCZRnb7&oauth_token_secret=qTTTJg3no25auiiWFerzjW4I"
			               "&oauth_callback_confirmed=true". */
			GHashTable *params = soup_form_decode (data);

			/* strdup()s are necessary because the hash table's set up to free keys. */
			if (g_hash_table_lookup (params, "oauth_token") != NULL) {
				g_hash_table_insert (params, (gpointer) g_strdup ("oauth_token"), (gpointer) "<redacted>");
			}
			if (g_hash_table_lookup (params, "oauth_token_secret") != NULL) {
				g_hash_table_insert (params, (gpointer) g_strdup ("oauth_token_secret"), (gpointer) "<redacted>");
			}

			_data = soup_form_encode_hash (params);

			g_hash_table_destroy (params);
		} else {
			/* Nothing to redact. */
			_data = g_strdup (data);
		}
	} else {
		/* Don't dupe the string. */
		_data = (gchar*) data;
	}

	/* Log the data. */
	g_debug ("%c %s", direction, _data);

	if (filter_data == TRUE) {
		g_free (_data);
	}
}