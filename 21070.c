archive_open_ready_cb (GObject      *source_object,
		       GAsyncResult *result,
		       gpointer      user_data)
{
	FrWindow  *window = user_data;
	FrArchive *archive;
	GError    *error = NULL;

	archive = fr_archive_open_finish (G_FILE (source_object), result, &error);
	_fr_window_set_archive (window, archive);

	g_signal_emit (window,
		       fr_window_signals[ARCHIVE_LOADED],
		       0,
		       error == NULL);

	_archive_operation_completed (window, FR_ACTION_LOADING_ARCHIVE, error);
	_g_error_free (error);
	_g_object_unref (archive);
}