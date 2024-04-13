fr_window_find (FrWindow *window,
		gboolean  active)
{
	if (active) {
		window->priv->filter_mode = TRUE;
		gtk_widget_show (window->priv->filter_bar);
		gtk_widget_hide (window->priv->location_bar);
		gtk_widget_grab_focus (window->priv->filter_entry);
	}
	else {
		window->priv->filter_mode = FALSE;
		window->priv->list_mode = window->priv->last_list_mode;

		gtk_entry_set_text (GTK_ENTRY (window->priv->filter_entry), "");
		gtk_widget_hide (window->priv->filter_bar);

		gtk_list_store_clear (window->priv->list_store);

		fr_window_update_columns_visibility (window);
		fr_window_update_file_list (window, TRUE);
		fr_window_update_dir_tree (window);
		fr_window_update_current_location (window);
		fr_window_go_to_location (window, gtk_entry_get_text (GTK_ENTRY (window->priv->location_entry)), FALSE);
	}
}