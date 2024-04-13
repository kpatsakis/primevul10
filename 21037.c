paste_from_archive_remove_ready_cb (GObject      *source_object,
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

	paste_from_archive_completed_successfully (window);
}