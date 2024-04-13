fr_window_archive_add_files (FrWindow   *window,
			     GList      *file_list, /* GFile list */
			     GFile      *base_dir,
			     gboolean    update)
{
	_archive_operation_started (window, FR_ACTION_ADDING_FILES);

	fr_archive_add_files (window->archive,
			      file_list,
			      base_dir,
			      fr_window_get_current_location (window),
			      update,
			      FALSE,
			      window->priv->password,
			      window->priv->encrypt_header,
			      window->priv->compression,
			      window->priv->volume_size,
			      window->priv->cancellable,
			      archive_add_files_ready_cb,
			      window);
}