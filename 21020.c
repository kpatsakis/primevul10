archive_extraction_ready_cb (GObject      *source_object,
			     GAsyncResult *result,
			     gpointer      user_data)
{
	ExtractData *edata = user_data;
	FrWindow    *window = edata->window;
	gboolean     ask_to_open_destination;
	gboolean     batch_mode;
	GError      *error = NULL;

	ask_to_open_destination = edata->ask_to_open_destination;
	batch_mode = window->priv->batch_mode;

	_g_clear_object (&window->priv->last_extraction_destination);
	window->priv->last_extraction_destination = _g_object_ref (fr_archive_get_last_extraction_destination (window->archive));

	fr_archive_operation_finish (FR_ARCHIVE (source_object), result, &error);
	_archive_operation_completed (window, FR_ACTION_EXTRACTING_FILES, error);

	if ((error == NULL) && ask_to_open_destination) {
		window->priv->quit_with_progress_dialog = window->priv->batch_mode;
		open_progress_dialog_with_open_destination (window);
	}
	else if ((error == NULL) && ! batch_mode && ! gtk_window_has_toplevel_focus (GTK_WINDOW (window->priv->progress_dialog)))
		gtk_window_present (GTK_WINDOW (window));

	_g_error_free (error);
}