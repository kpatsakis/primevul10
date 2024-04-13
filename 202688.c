vte_sequence_handler_change_color (VteTerminal *terminal, GValueArray *params)
{
	gchar **pairs, *str = NULL;
	GValue *value;
	GdkColor color;
	guint idx, i;

	if (params != NULL && params->n_values > 0) {
		value = g_value_array_get_nth (params, 0);

		if (G_VALUE_HOLDS_STRING (value))
			str = g_value_dup_string (value);
		else if (G_VALUE_HOLDS_POINTER (value))
			str = vte_ucs4_to_utf8 (terminal, g_value_get_pointer (value));

		if (! str)
			return;

		pairs = g_strsplit (str, ";", 0);
		if (! pairs) {
			g_free (str);
			return;
		}

		for (i = 0; pairs[i] && pairs[i + 1]; i += 2) {
			idx = strtoul (pairs[i], (char **) NULL, 10);

			if (idx >= VTE_DEF_FG)
				continue;

			if (vte_parse_color (pairs[i + 1], &color)) {
				terminal->pvt->palette[idx].red = color.red;
				terminal->pvt->palette[idx].green = color.green;
				terminal->pvt->palette[idx].blue = color.blue;
			} else if (strcmp (pairs[i + 1], "?") == 0) {
				gchar buf[128];
				g_snprintf (buf, sizeof (buf),
					    _VTE_CAP_OSC "4;%u;rgb:%04x/%04x/%04x" BEL, idx,
					    terminal->pvt->palette[idx].red,
					    terminal->pvt->palette[idx].green,
					    terminal->pvt->palette[idx].blue);
				vte_terminal_feed_child (terminal, buf, -1);
			}
		}

		g_free (str);
		g_strfreev (pairs);

		/* emit the refresh as the palette has changed and previous
		 * renders need to be updated. */
		vte_terminal_emit_refresh_window (terminal);
	}
}