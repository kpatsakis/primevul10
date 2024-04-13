theme_adium_constructed (GObject *object)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (object);
	const gchar           *font_family = NULL;
	gint                   font_size = 0;
	WebKitWebView         *webkit_view = WEBKIT_WEB_VIEW (object);
	WebKitWebInspector    *webkit_inspector;

	/* Set default settings */
	font_family = tp_asv_get_string (priv->data->info, "DefaultFontFamily");
	font_size = tp_asv_get_int32 (priv->data->info, "DefaultFontSize", NULL);

	if (font_family && font_size) {
		g_object_set (webkit_web_view_get_settings (webkit_view),
			"default-font-family", font_family,
			"default-font-size", font_size,
			NULL);
	} else {
		empathy_webkit_bind_font_setting (webkit_view,
			priv->gsettings_desktop,
			EMPATHY_PREFS_DESKTOP_INTERFACE_DOCUMENT_FONT_NAME);
	}

	/* Setup webkit inspector */
	webkit_inspector = webkit_web_view_get_inspector (webkit_view);
	g_signal_connect (webkit_inspector, "inspect-web-view",
			  G_CALLBACK (theme_adium_inspect_web_view_cb),
			  object);
	g_signal_connect (webkit_inspector, "show-window",
			  G_CALLBACK (theme_adium_inspector_show_window_cb),
			  object);
	g_signal_connect (webkit_inspector, "close-window",
			  G_CALLBACK (theme_adium_inspector_close_window_cb),
			  object);

	/* Load template */
	theme_adium_load_template (EMPATHY_THEME_ADIUM (object));

	priv->in_construction = FALSE;
}