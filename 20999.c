fr_window_close (FrWindow *window)
{
	if (window->priv->activity_ref > 0)
		return;

	if (window->priv->closing)
		return;

	window->priv->closing = TRUE;

	if (gtk_widget_get_realized (GTK_WIDGET (window))) {
		int width, height;

		width = gtk_widget_get_allocated_width (GTK_WIDGET (window));
		height = gtk_widget_get_allocated_height (GTK_WIDGET (window));
		g_settings_set_int (window->priv->settings_ui, PREF_UI_WINDOW_WIDTH, width);
		g_settings_set_int (window->priv->settings_ui, PREF_UI_WINDOW_HEIGHT, height);

		width = gtk_paned_get_position (GTK_PANED (window->priv->paned));
		if (width > 0)
			g_settings_set_int (window->priv->settings_ui, PREF_UI_SIDEBAR_WIDTH, width);

		width = gtk_tree_view_column_get_width (window->priv->filename_column);
		if (width > 0)
			g_settings_set_int (window->priv->settings_listing, PREF_LISTING_NAME_COLUMN_WIDTH, width);
	}

	gtk_widget_destroy (GTK_WIDGET (window));
}