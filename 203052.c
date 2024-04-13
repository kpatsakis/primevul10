empathy_theme_adium_show_inspector (EmpathyThemeAdium *theme)
{
	WebKitWebView      *web_view = WEBKIT_WEB_VIEW (theme);
	WebKitWebInspector *inspector;

	g_object_set (G_OBJECT (webkit_web_view_get_settings (web_view)),
		      "enable-developer-extras", TRUE,
		      NULL);

	inspector = webkit_web_view_get_inspector (web_view);
	webkit_web_inspector_show (inspector);
}