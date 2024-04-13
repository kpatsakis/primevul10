theme_adium_append_event (EmpathyChatView *view,
			  const gchar     *str)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (view);
	gchar *str_escaped;

	if (priv->pages_loading != 0) {
		queue_item (&priv->message_queue, QUEUED_EVENT, NULL, str);
		return;
	}

	str_escaped = g_markup_escape_text (str, -1);
	theme_adium_append_event_escaped (view, str_escaped);
	g_free (str_escaped);
}