fr_window_update_current_location (FrWindow *window)
{
	const char *current_dir = fr_window_get_current_location (window);
	char       *path;
	GtkTreeIter iter;

	if (window->priv->list_mode == FR_WINDOW_LIST_MODE_FLAT) {
		gtk_widget_hide (window->priv->location_bar);
		return;
	}

	gtk_widget_show (window->priv->location_bar);

	gtk_entry_set_text (GTK_ENTRY (window->priv->location_entry), window->priv->archive_present? current_dir: "");

	set_sensitive (window, "GoBack", window->priv->archive_present && (current_dir != NULL) && (window->priv->history_current != NULL) && (window->priv->history_current->next != NULL));
	set_sensitive (window, "GoForward", window->priv->archive_present && (current_dir != NULL) && (window->priv->history_current != NULL) && (window->priv->history_current->prev != NULL));
	set_sensitive (window, "GoUp", window->priv->archive_present && (current_dir != NULL) && (strcmp (current_dir, "/") != 0));
	set_sensitive (window, "GoHome", window->priv->archive_present);
	gtk_widget_set_sensitive (window->priv->location_entry, window->priv->archive_present);
	gtk_widget_set_sensitive (window->priv->location_label, window->priv->archive_present);
	gtk_widget_set_sensitive (window->priv->filter_entry, window->priv->archive_present);

	fr_window_deactivate_filter (window);

#if 0
	fr_window_history_print (window);
#endif

	path = _g_path_remove_ending_separator (current_dir);
	if (get_tree_iter_from_path (window, path, NULL, &iter)) {
		GtkTreeSelection *selection;
		GtkTreePath      *t_path;

		t_path = gtk_tree_model_get_path (GTK_TREE_MODEL (window->priv->tree_store), &iter);
		gtk_tree_view_expand_to_path (GTK_TREE_VIEW (window->priv->tree_view), t_path);
		gtk_tree_path_free (t_path);

		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (window->priv->tree_view));
		gtk_tree_selection_select_iter (selection, &iter);
	}
	g_free (path);
}