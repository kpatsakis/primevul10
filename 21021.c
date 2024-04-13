paste_from_archive_open_cb (GObject      *source_object,
			    GAsyncResult *result,
			    gpointer      user_data)
{
	FrWindow *window = user_data;
	GError   *error;

	_g_object_unref (window->priv->copy_from_archive);
	window->priv->copy_from_archive = fr_archive_open_finish (G_FILE (source_object), result, &error);
	if (window->priv->copy_from_archive == NULL) {
		_paste_from_archive_operation_completed (window, FR_ACTION_PASTING_FILES, error);
		g_error_free (error);
		return;
	}

	g_signal_connect (G_OBJECT (window->priv->copy_from_archive),
			  "progress",
			  G_CALLBACK (fr_archive_progress_cb),
			  window);
	g_signal_connect (G_OBJECT (window->priv->copy_from_archive),
			  "message",
			  G_CALLBACK (fr_archive_message_cb),
			  window);
	g_signal_connect (G_OBJECT (window->priv->copy_from_archive),
			  "start",
			  G_CALLBACK (fr_archive_start_cb),
			  window);
	g_signal_connect (G_OBJECT (window->priv->copy_from_archive),
			  "stoppable",
			  G_CALLBACK (fr_archive_stoppable_cb),
			  window);
	g_signal_connect (G_OBJECT (window->priv->copy_from_archive),
			  "working-archive",
			  G_CALLBACK (fr_window_working_archive_cb),
			  window);

	fr_archive_action_started (window->priv->copy_from_archive, FR_ACTION_LISTING_CONTENT);
	fr_archive_list (window->priv->copy_from_archive,
			 window->priv->clipboard_data->password,
			 window->priv->cancellable,
			 paste_from_archive_list_ready_cb,
			 window);
}