display_progress_dialog (gpointer data)
{
	FrWindow *window = data;

	if (window->priv->progress_timeout != 0)
		g_source_remove (window->priv->progress_timeout);

	if (window->priv->use_progress_dialog && (window->priv->progress_dialog != NULL)) {
		gtk_dialog_set_response_sensitive (GTK_DIALOG (window->priv->progress_dialog),
						   GTK_RESPONSE_OK,
						   window->priv->stoppable);
		if (! window->priv->batch_mode)
			gtk_window_present (GTK_WINDOW (window));
		gtk_widget_hide (window->priv->progress_bar);
		gtk_window_present (GTK_WINDOW (window->priv->progress_dialog));
		fr_archive_message_cb (NULL, window->priv->pd_last_message, window);
	}

	window->priv->progress_timeout = 0;

	return FALSE;
}