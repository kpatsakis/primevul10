fr_window_realize (GtkWidget *widget)
{
	FrWindow     *window = FR_WINDOW (widget);
	GIcon        *icon;
	GtkClipboard *clipboard;

	GTK_WIDGET_CLASS (fr_window_parent_class)->realize (widget);

	window->priv->list_icon_cache = gth_icon_cache_new_for_widget (GTK_WIDGET (window), GTK_ICON_SIZE_LARGE_TOOLBAR);
	window->priv->tree_icon_cache = gth_icon_cache_new_for_widget (GTK_WIDGET (window), GTK_ICON_SIZE_MENU);

	icon = g_content_type_get_icon ("text/plain");
	gth_icon_cache_set_fallback (window->priv->list_icon_cache, icon);
	gth_icon_cache_set_fallback (window->priv->tree_icon_cache, icon);
	g_object_unref (icon);

	clipboard = gtk_widget_get_clipboard (widget, FR_CLIPBOARD);
	g_signal_connect (clipboard,
			  "owner_change",
			  G_CALLBACK (clipboard_owner_change_cb),
			  window);

	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (window->priv->list_store),
					      g_settings_get_enum (window->priv->settings_listing, PREF_LISTING_SORT_METHOD),
					      g_settings_get_enum (window->priv->settings_listing, PREF_LISTING_SORT_TYPE));

	fr_window_update_dir_tree (window);
	fr_window_update_file_list (window, TRUE);
}