fr_window_set_list_mode (FrWindow         *window,
			 FrWindowListMode  list_mode)
{
	g_return_if_fail (window != NULL);

	if (list_mode == window->priv->list_mode)
		return;

	window->priv->list_mode = window->priv->last_list_mode = list_mode;
	if (window->priv->list_mode == FR_WINDOW_LIST_MODE_FLAT) {
		fr_window_history_clear (window);
		fr_window_history_add (window, "/");
	}

	g_settings_set_enum (window->priv->settings_listing, PREF_LISTING_LIST_MODE, window->priv->last_list_mode);
	g_settings_set_boolean (window->priv->settings_listing, PREF_LISTING_SHOW_PATH, (window->priv->list_mode == FR_WINDOW_LIST_MODE_FLAT));

	fr_window_update_file_list (window, TRUE);
	fr_window_update_dir_tree (window);
	fr_window_update_current_location (window);
}