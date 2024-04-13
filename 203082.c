theme_adium_clear (EmpathyChatView *view)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (view);

	theme_adium_load_template (EMPATHY_THEME_ADIUM (view));

	/* Clear last contact to avoid trying to add a 'joined'
	 * message when we don't have an insertion point. */
	if (priv->last_contact) {
		g_object_unref (priv->last_contact);
		priv->last_contact = NULL;
	}
}