archive_add_ready_for_conversion_cb (GObject      *source_object,
				     GAsyncResult *result,
				     gpointer      user_data)
{
	ConvertData *cdata = user_data;
	FrWindow    *window = cdata->window;
	GError      *error = NULL;

	fr_archive_operation_finish (FR_ARCHIVE (source_object), result, &error);

	_fr_window_stop_activity_mode (window);
	close_progress_dialog (window, FALSE);

	if (error != NULL) {
		_handle_archive_operation_error (window,
						 cdata->new_archive,
						 FR_ACTION_ADDING_FILES,
						 error,
						 NULL,
						 NULL);
		fr_window_stop_batch (window);
		g_error_free (error);
		return;
	}

	open_progress_dialog_with_open_archive (window);
	fr_window_exec_next_batch_action (window);
}