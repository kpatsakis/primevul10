fr_window_archive_add_with_filter (FrWindow      *window,
				   GList         *file_list, /* GFile list */
				   GFile         *base_dir,
				   const char    *include_files,
				   const char    *exclude_files,
				   const char    *exclude_folders,
				   const char    *dest_dir,
				   gboolean       update,
				   gboolean       follow_links)
{
	_archive_operation_started (window, FR_ACTION_ADDING_FILES);

	fr_archive_add_files_with_filter (window->archive,
					  file_list,
					  base_dir,
					  include_files,
					  exclude_files,
					  exclude_folders,
					  (dest_dir == NULL)? fr_window_get_current_location (window): dest_dir,
					  update,
					  follow_links,
					  window->priv->password,
					  window->priv->encrypt_header,
					  window->priv->compression,
					  window->priv->volume_size,
					  window->priv->cancellable,
					  archive_add_files_ready_cb,
					  window);
}