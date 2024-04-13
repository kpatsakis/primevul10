paste_from_archive_extract_ready_cb (GObject      *source_object,
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

	fr_archive_paste_clipboard (window->archive,
				    window->priv->clipboard_data->file,
				    window->priv->password,
				    window->priv->encrypt_header,
				    window->priv->compression,
				    window->priv->volume_size,
				    window->priv->clipboard_data->op,
				    window->priv->clipboard_data->base_dir,
				    window->priv->clipboard_data->files,
				    window->priv->clipboard_data->tmp_dir,
				    window->priv->clipboard_data->current_dir,
				    window->priv->cancellable,
				    paste_from_archive_paste_clipboard_ready_cb,
				    window);
}