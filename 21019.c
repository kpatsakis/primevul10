archive_add_ready_for_encryption_cb (GObject      *source_object,
				     GAsyncResult *result,
				     gpointer      user_data)
{
	EncryptData *edata = user_data;
	FrWindow    *window = edata->window;
	GError      *error = NULL;

	if (! fr_archive_operation_finish (FR_ARCHIVE (source_object), result, &error)) {
		_encrypt_operation_completed_with_error (window, FR_ACTION_ENCRYPTING_ARCHIVE, error);
		return;
	}

	fr_archive_action_started (window->archive, FR_ACTION_SAVING_REMOTE_ARCHIVE);
	g_file_copy_async (edata->temp_new_file,
			   fr_archive_get_file (window->archive),
			   G_FILE_COPY_OVERWRITE,
			   G_PRIORITY_DEFAULT,
			   window->priv->cancellable,
			   encryption_copy_progress_cb,
			   edata,
			   ecryption_copy_ready_cb,
			   edata);
}