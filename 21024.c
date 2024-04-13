fr_window_paste_from_clipboard_data (FrWindow        *window,
				     FrClipboardData *data)
{
	const char *current_dir_relative;
	GHashTable *created_dirs;
	GList      *scan;
	char       *from_archive;
	char       *to_archive;

	if (window->priv->second_password != NULL)
		fr_clipboard_data_set_password (data, window->priv->second_password);

	if (window->priv->clipboard_data != data) {
		fr_clipboard_data_unref (window->priv->clipboard_data);
		window->priv->clipboard_data = data;
	}

	fr_window_set_current_batch_action (window,
					    FR_BATCH_ACTION_PASTE,
					    fr_clipboard_data_ref (data),
					    (GFreeFunc) fr_clipboard_data_unref);

	current_dir_relative = data->current_dir + 1;

	data->tmp_dir = _g_file_get_temp_work_dir (NULL);
	created_dirs = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
	for (scan = data->files; scan; scan = scan->next) {
		const char *old_name = (char*) scan->data;
		char       *new_name;
		char       *dir;

		new_name = g_build_filename (current_dir_relative, old_name + strlen (data->base_dir) - 1, NULL);
		dir = _g_path_remove_level (new_name);
		if ((dir != NULL) && (g_hash_table_lookup (created_dirs, dir) == NULL)) {
			GFile *directory;

			directory = _g_file_append_path (data->tmp_dir, dir, NULL);
			debug (DEBUG_INFO, "mktree %s\n", g_file_get_uri (directory));
			_g_file_make_directory_tree (directory, 0700, NULL);

			g_hash_table_replace (created_dirs, g_strdup (dir), GINT_TO_POINTER (1));
		}

		g_free (dir);
		g_free (new_name);
	}
	g_hash_table_destroy (created_dirs);

	/**/

	g_free (window->priv->custom_action_message);
	from_archive = _g_file_get_display_basename (data->file);
	to_archive = _g_file_get_display_basename (window->priv->archive_file);
	if (data->op == FR_CLIPBOARD_OP_CUT)
		/* Translators: %s are archive filenames */
		window->priv->custom_action_message = g_strdup_printf (_("Moving the files from \"%s\" to \"%s\""), from_archive, to_archive);
	else
		/* Translators: %s are archive filenames */
		window->priv->custom_action_message = g_strdup_printf (_("Copying the files from \"%s\" to \"%s\""), from_archive, to_archive);
	_archive_operation_started (window, FR_ACTION_PASTING_FILES);

	_window_started_loading_file (window, data->file);
	fr_archive_open (data->file,
			 window->priv->cancellable,
			 paste_from_archive_open_cb,
			 window);

	g_free (to_archive);
	g_free (from_archive);
}