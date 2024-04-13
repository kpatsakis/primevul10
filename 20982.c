archive_extraction_ready_for_encryption_cb (GObject      *source_object,
					    GAsyncResult *result,
					    gpointer      user_data)
{
	EncryptData *edata = user_data;
	FrWindow    *window = edata->window;
	GList       *list;
	GError      *error = NULL;

	if (! fr_archive_operation_finish (FR_ARCHIVE (source_object), result, &error)) {
		_encrypt_operation_completed_with_error (window, FR_ACTION_ENCRYPTING_ARCHIVE, error);
		return;
	}

	fr_archive_action_started (window->archive, FR_ACTION_ENCRYPTING_ARCHIVE);

	list = g_list_prepend (NULL, edata->temp_extraction_dir);
	fr_archive_add_files (edata->new_archive,
			      list,
			      edata->temp_extraction_dir,
			      NULL,
			      FALSE,
			      FALSE,
			      edata->password,
			      edata->encrypt_header,
			      window->priv->compression,
			      0,
			      window->priv->cancellable,
			      archive_add_ready_for_encryption_cb,
			      edata);

	g_list_free (list);
}