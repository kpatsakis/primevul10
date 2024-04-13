fr_window_update_dir_tree (FrWindow *window)
{
	GPtrArray  *dirs;
	GHashTable *dir_cache;
	int         i;
	GdkPixbuf  *icon;

	if (! gtk_widget_get_realized (GTK_WIDGET (window)))
		return;

	gtk_tree_store_clear (window->priv->tree_store);

	if (! window->priv->view_folders
	    || ! window->priv->archive_present
	    || (window->priv->list_mode == FR_WINDOW_LIST_MODE_FLAT))
	{
		gtk_widget_set_sensitive (window->priv->tree_view, FALSE);
		gtk_widget_hide (window->priv->sidepane);
		return;
	}
	else {
		gtk_widget_set_sensitive (window->priv->tree_view, TRUE);
		if (! gtk_widget_get_visible (window->priv->sidepane))
			gtk_widget_show_all (window->priv->sidepane);
	}

	if (gtk_widget_get_realized (window->priv->tree_view))
		gtk_tree_view_scroll_to_point (GTK_TREE_VIEW (window->priv->tree_view), 0, 0);

	/**/

	dirs = g_ptr_array_sized_new (128);

	dir_cache = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, NULL);
	for (i = 0; i < window->archive->files->len; i++) {
		FileData *fdata = g_ptr_array_index (window->archive->files, i);
		char     *dir;

		if (gtk_entry_get_text (GTK_ENTRY (window->priv->filter_entry)) != NULL) {
			if (! file_data_respects_filter (window, fdata))
				continue;
		}

		if (fdata->dir)
			dir = _g_path_remove_ending_separator (fdata->full_path);
		else
			dir = _g_path_remove_level (fdata->full_path);

		while ((dir != NULL) && (strcmp (dir, "/") != 0)) {
			char *new_dir;

			if (g_hash_table_lookup (dir_cache, dir) != NULL)
				break;

			new_dir = dir;
			g_ptr_array_add (dirs, new_dir);
			g_hash_table_replace (dir_cache, new_dir, "1");

			dir = _g_path_remove_level (new_dir);
		}

		g_free (dir);
	}
	g_hash_table_destroy (dir_cache);

	g_ptr_array_sort (dirs, path_compare);
	dir_cache = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, (GDestroyNotify) gtk_tree_path_free);

	/**/

	icon = get_mime_type_icon (window, MIME_TYPE_ARCHIVE);
	{
		GtkTreeIter  node;
		char        *name;

		name = _g_file_get_display_basename (fr_archive_get_file (window->archive));

		gtk_tree_store_append (window->priv->tree_store, &node, NULL);
		gtk_tree_store_set (window->priv->tree_store, &node,
				    TREE_COLUMN_ICON, icon,
				    TREE_COLUMN_NAME, name,
				    TREE_COLUMN_PATH, "/",
				    TREE_COLUMN_WEIGHT, PANGO_WEIGHT_BOLD,
				    -1);
		g_hash_table_replace (dir_cache, "/", gtk_tree_model_get_path (GTK_TREE_MODEL (window->priv->tree_store), &node));

		g_free (name);
	}
	g_object_unref (icon);

	/**/

	icon = get_mime_type_icon (window, MIME_TYPE_DIRECTORY);
	for (i = 0; i < dirs->len; i++) {
		char        *dir = g_ptr_array_index (dirs, i);
		char        *parent_dir;
		GtkTreePath *parent_path;
		GtkTreeIter  parent;
		GtkTreeIter  node;

		parent_dir = _g_path_remove_level (dir);
		if (parent_dir == NULL)
			continue;

		parent_path = g_hash_table_lookup (dir_cache, parent_dir);
		gtk_tree_model_get_iter (GTK_TREE_MODEL (window->priv->tree_store),
					 &parent,
					 parent_path);
		gtk_tree_store_append (window->priv->tree_store, &node, &parent);
		gtk_tree_store_set (window->priv->tree_store, &node,
				    TREE_COLUMN_ICON, icon,
				    TREE_COLUMN_NAME, _g_path_get_basename (dir),
				    TREE_COLUMN_PATH, dir,
				    TREE_COLUMN_WEIGHT, PANGO_WEIGHT_NORMAL,
				    -1);
		g_hash_table_replace (dir_cache, dir, gtk_tree_model_get_path (GTK_TREE_MODEL (window->priv->tree_store), &node));

		g_free (parent_dir);
	}
	g_hash_table_destroy (dir_cache);
	if (icon != NULL)
		g_object_unref (icon);

	g_ptr_array_free (dirs, TRUE);

	fr_window_update_current_location (window);
}