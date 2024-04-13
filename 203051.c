theme_adium_parse_body (EmpathyThemeAdium *self,
	const gchar *text,
	const gchar *token)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (self);
	EmpathyStringParser *parsers;
	GString *string;

	/* Check if we have to parse smileys */
	parsers = empathy_webkit_get_string_parser (
		g_settings_get_boolean (priv->gsettings_chat,
			EMPATHY_PREFS_CHAT_SHOW_SMILEYS));

	/* Parse text and construct string with links and smileys replaced
	 * by html tags. Also escape text to make sure html code is
	 * displayed verbatim. */
	string = g_string_sized_new (strlen (text));

	/* wrap this in HTML that allows us to find the message for later
	 * editing */
	if (!tp_str_empty (token))
		g_string_append_printf (string,
			"<span id=\"message-token-%s\">",
			token);

	empathy_string_parser_substr (text, -1, parsers, string);

	if (!tp_str_empty (token))
		g_string_append (string, "</span>");

	/* Wrap body in order to make tabs and multiple spaces displayed
	 * properly. See bug #625745. */
	g_string_prepend (string, "<div style=\"display: inline; "
					       "white-space: pre-wrap\"'>");
	g_string_append (string, "</div>");

	return g_string_free (string, FALSE);
}