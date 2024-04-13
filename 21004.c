pref_list_mode_changed (GSettings  *settings,
			const char *key,
			gpointer    user_data)
{
	FrWindow *window = user_data;

	fr_window_set_list_mode (window, g_settings_get_enum (settings, key));
}