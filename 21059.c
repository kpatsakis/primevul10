theme_changed_cb (GtkIconTheme *theme,
		  FrWindow     *window)
{
	if (window->priv->populating_file_list)
		return;

	gth_icon_cache_clear (window->priv->list_icon_cache);
	gth_icon_cache_clear (window->priv->tree_icon_cache);

	fr_window_update_file_list (window, TRUE);
	fr_window_update_dir_tree (window);
}