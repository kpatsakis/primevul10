open_files_extract_ready_cb (GObject      *source_object,
			     GAsyncResult *result,
			     gpointer      user_data)
{
	OpenFilesData *odata = user_data;
	GError        *error = NULL;

	open_files_data_ref (odata);
	fr_archive_operation_finish (FR_ARCHIVE (source_object), result, &error);
	_archive_operation_completed (odata->window, FR_ACTION_EXTRACTING_FILES, error);

	if (error == NULL)
		fr_window_open_extracted_files (odata);

	open_files_data_unref (odata);
	_g_error_free (error);
}