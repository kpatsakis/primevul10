theme_adium_update_enable_webkit_developer_tools (EmpathyThemeAdium *theme)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (theme);
	WebKitWebView  *web_view = WEBKIT_WEB_VIEW (theme);
	gboolean        enable_webkit_developer_tools;

	enable_webkit_developer_tools = g_settings_get_boolean (
			priv->gsettings_chat,
			EMPATHY_PREFS_CHAT_WEBKIT_DEVELOPER_TOOLS);

	g_object_set (G_OBJECT (webkit_web_view_get_settings (web_view)),
		      "enable-developer-extras",
		      enable_webkit_developer_tools,
		      NULL);
}