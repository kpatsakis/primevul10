archive_extraction_ready_for_convertion_cb (GObject      *source_object,
					    GAsyncResult *result,
					    gpointer      user_data)
{
	ConvertData *cdata = user_data;
	FrWindow    *window = cdata->window;
	GList       *list;
	GError      *error = NULL;

	if (! fr_archive_operation_finish (FR_ARCHIVE (source_object), result, &error)) {
		_convertion_completed_with_error (window, FR_ACTION_EXTRACTING_FILES, error);
		return;
	}

	list = g_list_prepend (NULL, cdata->temp_extraction_dir);
	fr_archive_add_files (cdata->new_archive,
			      list,
			      cdata->temp_extraction_dir,
			      NULL,
			      FALSE,
			      FALSE,
			      cdata->password,
			      cdata->encrypt_header,
			      window->priv->compression,
			      cdata->volume_size,
			      window->priv->cancellable,
			      archive_add_ready_for_conversion_cb,
			      cdata);

	g_list_free (list);
}