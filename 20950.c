paste_from_archive_list_ready_cb (GObject      *source_object,
				  GAsyncResult *result,
				  gpointer      user_data)
{
	FrWindow *window = user_data;
	GError   *error = NULL;

	if (! fr_archive_operation_finish (FR_ARCHIVE (source_object), result, &error)) {
		_paste_from_archive_operation_completed (window, FR_ACTION_PASTING_FILES, error);
		g_error_free (error);
		return;
	}

	fr_archive_action_started (window->priv->copy_from_archive, FR_ACTION_EXTRACTING_FILES);
	fr_archive_extract (window->priv->copy_from_archive,
			    window->priv->clipboard_data->files,
			    window->priv->clipboard_data->tmp_dir,
			    NULL,
			    FALSE,
			    TRUE,
			    FALSE,
			    window->priv->clipboard_data->password,
			    window->priv->cancellable,
			    paste_from_archive_extract_ready_cb,
			    window);
}