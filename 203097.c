theme_adium_append_html (EmpathyThemeAdium *theme,
			 const gchar       *func,
			 const gchar       *html,
		         const gchar       *message,
		         const gchar       *avatar_filename,
		         const gchar       *name,
		         const gchar       *contact_id,
		         const gchar       *service_name,
		         const gchar       *message_classes,
		         gint64             timestamp,
		         gboolean           is_backlog,
		         gboolean           outgoing)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (theme);
	GString     *string;
	const gchar *cur = NULL;
	gchar       *script;

	/* Make some search-and-replace in the html code */
	string = g_string_sized_new (strlen (html) + strlen (message));
	g_string_append_printf (string, "%s(\"", func);
	for (cur = html; *cur != '\0'; cur++) {
		const gchar *replace = NULL;
		gchar       *dup_replace = NULL;
		gchar       *format = NULL;

		/* Those are all well known keywords that needs replacement in
		 * html files. Please keep them in the same order than the adium
		 * spec. See http://trac.adium.im/wiki/CreatingMessageStyles */
		if (theme_adium_match (&cur, "%userIconPath%")) {
			replace = avatar_filename;
		} else if (theme_adium_match (&cur, "%senderScreenName%")) {
			replace = contact_id;
		} else if (theme_adium_match (&cur, "%sender%")) {
			replace = name;
		} else if (theme_adium_match (&cur, "%senderColor%")) {
			/* A color derived from the user's name.
			 * FIXME: If a colon separated list of HTML colors is at
			 * Incoming/SenderColors.txt it will be used instead of
			 * the default colors.
			 */

			/* Ensure we always use the same color when sending messages
			 * (bgo #658821) */
			if (outgoing) {
				replace = "inherit";
			} else if (contact_id != NULL) {
				guint hash = g_str_hash (contact_id);
				replace = colors[hash % G_N_ELEMENTS (colors)];
			}
		} else if (theme_adium_match (&cur, "%senderStatusIcon%")) {
			/* FIXME: The path to the status icon of the sender
			 * (available, away, etc...)
			 */
		} else if (theme_adium_match (&cur, "%messageDirection%")) {
			/* FIXME: The text direction of the message
			 * (either rtl or ltr)
			 */
		} else if (theme_adium_match (&cur, "%senderDisplayName%")) {
			/* FIXME: The serverside (remotely set) name of the
			 * sender, such as an MSN display name.
			 *
			 *  We don't have access to that yet so we use
			 * local alias instead.
			 */
			replace = name;
		} else if (theme_adium_match_with_format (&cur, "%textbackgroundcolor{", &format)) {
			/* FIXME: This keyword is used to represent the
			 * highlight background color. "X" is the opacity of the
			 * background, ranges from 0 to 1 and can be any decimal
			 * between.
			 */
		} else if (theme_adium_match (&cur, "%message%")) {
			replace = message;
		} else if (theme_adium_match (&cur, "%time%") ||
			   theme_adium_match_with_format (&cur, "%time{", &format)) {
			const gchar *strftime_format;

			strftime_format = nsdate_to_strftime (priv->data, format);
			if (is_backlog) {
				dup_replace = empathy_time_to_string_local (timestamp,
					strftime_format ? strftime_format :
					EMPATHY_TIME_DATE_FORMAT_DISPLAY_SHORT);
			} else {
				dup_replace = empathy_time_to_string_local (timestamp,
					strftime_format ? strftime_format :
					EMPATHY_TIME_FORMAT_DISPLAY_SHORT);
			}
			replace = dup_replace;
		} else if (theme_adium_match (&cur, "%shortTime%")) {
			dup_replace = empathy_time_to_string_local (timestamp,
				EMPATHY_TIME_FORMAT_DISPLAY_SHORT);
			replace = dup_replace;
		} else if (theme_adium_match (&cur, "%service%")) {
			replace = service_name;
		} else if (theme_adium_match (&cur, "%variant%")) {
			/* FIXME: The name of the active message style variant,
			 * with all spaces replaced with an underscore.
			 * A variant named "Alternating Messages - Blue Red"
			 * will become "Alternating_Messages_-_Blue_Red".
			 */
		} else if (theme_adium_match (&cur, "%userIcons%")) {
			/* FIXME: mus t be "hideIcons" if use preference is set
			 * to hide avatars */
			replace = "showIcons";
		} else if (theme_adium_match (&cur, "%messageClasses%")) {
			replace = message_classes;
		} else if (theme_adium_match (&cur, "%status%")) {
			/* FIXME: A description of the status event. This is
			 * neither in the user's local language nor expected to
			 * be displayed; it may be useful to use a different div
			 * class to present different types of status messages.
			 * The following is a list of some of the more important
			 * status messages; your message style should be able to
			 * handle being shown a status message not in this list,
			 * as even at present the list is incomplete and is
			 * certain to become out of date in the future:
			 * 	online
			 *	offline
			 *	away
			 *	away_message
			 *	return_away
			 *	idle
			 *	return_idle
			 *	date_separator
			 *	contact_joined (group chats)
			 *	contact_left
			 *	error
			 *	timed_out
			 *	encryption (all OTR messages use this status)
			 *	purple (all IRC topic and join/part messages use this status)
			 *	fileTransferStarted
			 *	fileTransferCompleted
			 */
		} else {
			escape_and_append_len (string, cur, 1);
			continue;
		}

		/* Here we have a replacement to make */
		escape_and_append_len (string, replace, -1);

		g_free (dup_replace);
		g_free (format);
	}
	g_string_append (string, "\")");

	script = g_string_free (string, FALSE);
	webkit_web_view_execute_script (WEBKIT_WEB_VIEW (theme), script);
	g_free (script);
}