fr_window_unmap (GtkWidget *widget)
{
	FrWindow    *window = FR_WINDOW (widget);
	GtkSortType  order;
	int          column_id;

	if (gtk_tree_sortable_get_sort_column_id (GTK_TREE_SORTABLE (window->priv->list_store),
						  &column_id,
						  &order))
	{
		g_settings_set_enum (window->priv->settings_listing, PREF_LISTING_SORT_METHOD, column_id);
		g_settings_set_enum (window->priv->settings_listing, PREF_LISTING_SORT_TYPE, order);
	}

	GTK_WIDGET_CLASS (fr_window_parent_class)->unmap (widget);
}