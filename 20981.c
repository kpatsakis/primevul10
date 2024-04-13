ecryption_copy_ready_cb (GObject      *source_object,
			 GAsyncResult *result,
			 gpointer      user_data)
{
	EncryptData *edata = user_data;
	FrWindow    *window = edata->window;
	GError      *error = NULL;

	_fr_window_stop_activity_mode (window);
	close_progress_dialog (window, FALSE);

	if (! g_file_copy_finish (G_FILE (source_object), result, &error)) {
		_handle_archive_operation_error (window,
						 edata->new_archive,
						 FR_ACTION_CREATING_NEW_ARCHIVE,
						 error,
						 NULL,
						 NULL);
		fr_window_stop_batch (window);

		g_error_free (error);
		return;
	}

	fr_window_set_password (window, edata->password);
	fr_window_set_encrypt_header (window, edata->encrypt_header);
	window->priv->reload_archive = TRUE;
	fr_window_exec_next_batch_action (window);
}