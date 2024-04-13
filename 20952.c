paste_from_archive_paste_clipboard_ready_cb (GObject      *source_object,
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

	if (window->priv->clipboard_data->op == FR_CLIPBOARD_OP_CUT) {
		fr_archive_action_started (window->priv->copy_from_archive, FR_ACTION_DELETING_FILES);
		fr_archive_remove (window->priv->copy_from_archive,
				   window->priv->clipboard_data->files,
				   window->priv->compression,
				   window->priv->cancellable,
				   paste_from_archive_remove_ready_cb,
				   window);
	}
	else
		paste_from_archive_completed_successfully (window);
}