fr_window_show_cb (GtkWidget *widget,
		   FrWindow  *window)
{
	fr_window_update_current_location (window);

	set_active (window, "ViewStatusbar", g_settings_get_boolean (window->priv->settings_ui, PREF_UI_VIEW_STATUSBAR));

	window->priv->view_folders = g_settings_get_boolean (window->priv->settings_ui, PREF_UI_VIEW_FOLDERS);
	set_active (window, "ViewFolders", window->priv->view_folders);

	gtk_widget_hide (window->priv->filter_bar);

	return TRUE;
}