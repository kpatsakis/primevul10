empathy_theme_adium_init (EmpathyThemeAdium *theme)
{
	EmpathyThemeAdiumPriv *priv = G_TYPE_INSTANCE_GET_PRIVATE (theme,
		EMPATHY_TYPE_THEME_ADIUM, EmpathyThemeAdiumPriv);

	theme->priv = priv;

	priv->in_construction = TRUE;
	g_queue_init (&priv->message_queue);
	priv->allow_scrolling = TRUE;
	priv->smiley_manager = empathy_smiley_manager_dup_singleton ();

	g_signal_connect (theme, "load-finished",
			  G_CALLBACK (theme_adium_load_finished_cb),
			  NULL);
	g_signal_connect (theme, "navigation-policy-decision-requested",
			  G_CALLBACK (theme_adium_navigation_policy_decision_requested_cb),
			  NULL);

	priv->gsettings_chat = g_settings_new (EMPATHY_PREFS_CHAT_SCHEMA);
	priv->gsettings_desktop = g_settings_new (
		EMPATHY_PREFS_DESKTOP_INTERFACE_SCHEMA);

	g_signal_connect (priv->gsettings_chat,
		"changed::" EMPATHY_PREFS_CHAT_WEBKIT_DEVELOPER_TOOLS,
		G_CALLBACK (theme_adium_notify_enable_webkit_developer_tools_cb),
		theme);

	theme_adium_update_enable_webkit_developer_tools (theme);
}