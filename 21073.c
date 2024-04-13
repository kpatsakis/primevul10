fr_window_update_files (FrWindow *window,
			GList    *open_file_list)
{
	GList *file_list;
	GList *dir_list;
	GList *scan;

	if (window->priv->activity_ref > 0)
		return FALSE;

	if (window->archive->read_only)
		return FALSE;

	/* the size will be computed by the archive object */
	window->archive->files_to_add_size = 0;

	file_list = NULL;
	dir_list = NULL;
	for (scan = open_file_list; scan; scan = scan->next) {
		OpenFile *open_file = scan->data;

		file_list = g_list_prepend (file_list, g_object_ref (open_file->extracted_file));
		dir_list = g_list_prepend (dir_list, g_object_ref (open_file->temp_dir));
	}

	_archive_operation_started (window, FR_ACTION_UPDATING_FILES);

	fr_archive_update_open_files (window->archive,
				      file_list,
				      dir_list,
				      window->priv->password,
				      window->priv->encrypt_header,
				      window->priv->compression,
				      window->priv->volume_size,
				      window->priv->cancellable,
				      update_files_ready_cb,
				      window);

	_g_object_list_unref (dir_list);
	_g_object_list_unref (file_list);

	return TRUE;
}