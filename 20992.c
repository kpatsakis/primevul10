fr_window_activate_filter (FrWindow *window)
{
	GtkTreeView       *tree_view = GTK_TREE_VIEW (window->priv->list_view);
	GtkTreeViewColumn *column;

	gtk_widget_show (window->priv->filter_bar);
	window->priv->list_mode = FR_WINDOW_LIST_MODE_FLAT;

	gtk_list_store_clear (window->priv->list_store);

	column = gtk_tree_view_get_column (tree_view, 4);
	gtk_tree_view_column_set_visible (column, TRUE);

	fr_window_update_file_list (window, TRUE);
	fr_window_update_dir_tree (window);
	fr_window_update_current_location (window);
}