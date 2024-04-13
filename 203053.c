theme_adium_notify_enable_webkit_developer_tools_cb (GSettings   *gsettings,
						     const gchar *key,
						     gpointer     user_data)
{
	EmpathyThemeAdium  *theme = user_data;

	theme_adium_update_enable_webkit_developer_tools (theme);
}