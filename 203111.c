theme_adium_append_event_escaped (EmpathyChatView *view,
				  const gchar     *escaped)
{
	EmpathyThemeAdium     *theme = EMPATHY_THEME_ADIUM (view);
	EmpathyThemeAdiumPriv *priv = GET_PRIV (theme);

	theme_adium_append_html (theme, "appendMessage",
				 priv->data->status_html, escaped, NULL, NULL, NULL,
				 NULL, "event",
				 empathy_time_get_current (), FALSE, FALSE);

	/* There is no last contact */
	if (priv->last_contact) {
		g_object_unref (priv->last_contact);
		priv->last_contact = NULL;
	}
}