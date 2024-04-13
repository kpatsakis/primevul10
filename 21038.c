progress_dialog_response (GtkDialog *dialog,
			  int        response_id,
			  FrWindow  *window)
{
	GtkWidget *new_window;
	GFile     *saved_file;

	saved_file = window->priv->saving_file;
	window->priv->saving_file = NULL;

	switch (response_id) {
	case GTK_RESPONSE_CANCEL:
		if (window->priv->stoppable) {
			activate_action_stop (NULL, window);
			close_progress_dialog (window, TRUE);
		}
		break;
	case GTK_RESPONSE_CLOSE:
		close_progress_dialog (window, TRUE);
		break;
	case DIALOG_RESPONSE_OPEN_ARCHIVE:
		new_window = fr_window_new ();
		gtk_widget_show (new_window);
		fr_window_archive_open (FR_WINDOW (new_window), saved_file, GTK_WINDOW (new_window));
		close_progress_dialog (window, TRUE);
		break;
	case DIALOG_RESPONSE_OPEN_DESTINATION_FOLDER:
		fr_window_view_extraction_destination_folder (window);
		close_progress_dialog (window, TRUE);
		break;
	case DIALOG_RESPONSE_QUIT:
		g_idle_add (close_window_cb, window);
		break;
	default:
		break;
	}

	_g_object_unref (saved_file);
}