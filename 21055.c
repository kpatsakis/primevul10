fr_window_update_file_list (FrWindow *window,
			    gboolean  update_view)
{
	GPtrArray  *files;
	gboolean    free_files = FALSE;

	if (! gtk_widget_get_realized (GTK_WIDGET (window)))
		return;

	if (gtk_widget_get_realized (window->priv->list_view))
		gtk_tree_view_scroll_to_point (GTK_TREE_VIEW (window->priv->list_view), 0, 0);

	if (! window->priv->archive_present || window->priv->archive_new) {
		if (update_view)
			gtk_list_store_clear (window->priv->list_store);

		window->priv->current_view_length = 0;

		if (window->priv->archive_new) {
			gtk_widget_set_sensitive (window->priv->list_view, TRUE);
			gtk_widget_show_all (gtk_widget_get_parent (window->priv->list_view));
		}
		else {
			gtk_widget_set_sensitive (window->priv->list_view, FALSE);
			gtk_widget_hide (gtk_widget_get_parent (window->priv->list_view));
		}

		return;
	}
	else {
		gtk_widget_set_sensitive (window->priv->list_view, TRUE);
		gtk_widget_show_all (gtk_widget_get_parent (window->priv->list_view));
	}

	if (window->priv->give_focus_to_the_list) {
		gtk_widget_grab_focus (window->priv->list_view);
		window->priv->give_focus_to_the_list = FALSE;
	}

	/**/

	_fr_window_start_activity_mode (window);

	if (window->priv->list_mode == FR_WINDOW_LIST_MODE_FLAT) {
		fr_window_compute_list_names (window, window->archive->files);
		files = window->archive->files;
		free_files = FALSE;
	}
	else {
		char *current_dir = g_strdup (fr_window_get_current_location (window));

		while (! fr_window_dir_exists_in_archive (window, current_dir)) {
			char *tmp;

			fr_window_history_pop (window);

			tmp = get_parent_dir (current_dir);
			g_free (current_dir);
			current_dir = tmp;

			fr_window_history_add (window, current_dir);
		}
		g_free (current_dir);

		fr_window_compute_list_names (window, window->archive->files);
		files = fr_window_get_current_dir_list (window);
		free_files = TRUE;
	}

	if (files != NULL)
		window->priv->current_view_length = files->len;
	else
		window->priv->current_view_length = 0;

	if (update_view)
		fr_window_populate_file_list (window, files);

	if (free_files)
		g_ptr_array_free (files, TRUE);
}