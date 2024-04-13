fr_window_populate_file_list (FrWindow  *window,
			      GPtrArray *files)
{
	int         sort_column_id;
	GtkSortType order;
	int         i;

	if (! gtk_widget_get_realized (GTK_WIDGET (window))) {
		_fr_window_stop_activity_mode (window);
		return;
	}

	window->priv->populating_file_list = TRUE;
	gtk_list_store_clear (window->priv->list_store);

	gtk_tree_sortable_get_sort_column_id (GTK_TREE_SORTABLE (window->priv->list_store),
					      &sort_column_id,
					      &order);
	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (window->priv->list_store),
	 				      GTK_TREE_SORTABLE_UNSORTED_SORT_COLUMN_ID,
	 				      0);

	for (i = 0; i < files->len; i++) {
		FileData    *fdata = g_ptr_array_index (files, i);
		GtkTreeIter  iter;
		GdkPixbuf   *icon, *emblem;
		char        *utf8_name;

		if (fdata->list_name == NULL)
			continue;

		gtk_list_store_append (window->priv->list_store, &iter);

		icon = get_icon (window, fdata);
		emblem = get_emblem (window, fdata);
		utf8_name = g_filename_display_name (fdata->list_name);

		if (file_data_is_dir (fdata)) {
			char *utf8_path;
			char *tmp;
			char *s_size;
			char *s_time;

			if (fdata->list_dir)
				tmp = _g_path_remove_ending_separator (fr_window_get_current_location (window));

			else
				tmp = _g_path_remove_level (fdata->path);
			utf8_path = g_filename_display_name (tmp);
			g_free (tmp);

			s_size = g_format_size (fdata->dir_size);

			if (fdata->list_dir)
				s_time = g_strdup ("");
			else
				s_time = _g_time_to_string (fdata->modified);

			gtk_list_store_set (window->priv->list_store, &iter,
					    COLUMN_FILE_DATA, fdata,
					    COLUMN_ICON, icon,
					    COLUMN_NAME, utf8_name,
					    COLUMN_EMBLEM, emblem,
					    COLUMN_TYPE, _("Folder"),
					    COLUMN_SIZE, s_size,
					    COLUMN_TIME, s_time,
					    COLUMN_PATH, utf8_path,
					    -1);
			g_free (utf8_path);
			g_free (s_size);
			g_free (s_time);
		}
		else {
			char       *utf8_path;
			char       *s_size;
			char       *s_time;
			const char *desc;

			utf8_path = g_filename_display_name (fdata->path);

			s_size = g_format_size (fdata->size);
			s_time = _g_time_to_string (fdata->modified);
			desc = g_content_type_get_description (fdata->content_type);

			gtk_list_store_set (window->priv->list_store, &iter,
					    COLUMN_FILE_DATA, fdata,
					    COLUMN_ICON, icon,
					    COLUMN_NAME, utf8_name,
					    COLUMN_EMBLEM, emblem,
					    COLUMN_TYPE, desc,
					    COLUMN_SIZE, s_size,
					    COLUMN_TIME, s_time,
					    COLUMN_PATH, utf8_path,
					    -1);
			g_free (utf8_path);
			g_free (s_size);
			g_free (s_time);
		}

		g_free (utf8_name);
		_g_object_unref (icon);
		_g_object_unref (emblem);
	}

	gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (window->priv->list_store),
					      sort_column_id,
					      order);

	window->priv->populating_file_list = FALSE;

	_fr_window_stop_activity_mode (window);
}