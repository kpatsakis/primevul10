archive_add_files_ready_cb (GObject      *source_object,
			    GAsyncResult *result,
			    gpointer      user_data)
{
	FrWindow *window = user_data;
	gboolean  notify;
	GError   *error = NULL;

	notify = window->priv->notify;

	fr_archive_operation_finish (FR_ARCHIVE (source_object), result, &error);
	_archive_operation_completed (window, FR_ACTION_ADDING_FILES, error);

	if ((error == NULL) && notify) {
		window->priv->quit_with_progress_dialog = TRUE;
		open_progress_dialog_with_open_archive (window);

		if (! gtk_window_has_toplevel_focus (GTK_WINDOW (window->priv->progress_dialog)))
			_fr_window_notify_creation_complete (window);
	}

	_g_error_free (error);
}