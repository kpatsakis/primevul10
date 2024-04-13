open_progress_dialog_with_open_archive (FrWindow *window)
{
	char *basename;
	char *description;

	if (window->priv->hide_progress_timeout != 0) {
		g_source_remove (window->priv->hide_progress_timeout);
		window->priv->hide_progress_timeout = 0;
	}
	if (window->priv->progress_timeout != 0) {
		g_source_remove (window->priv->progress_timeout);
		window->priv->progress_timeout = 0;
	}

	create_the_progress_dialog (window);
	gtk_widget_hide (window->priv->pd_cancel_button);
	gtk_widget_hide (window->priv->pd_open_destination_button);
	gtk_widget_show (window->priv->pd_open_archive_button);
	gtk_widget_set_visible (window->priv->pd_quit_button, ! window->priv->quit_with_progress_dialog);
	gtk_widget_show (window->priv->pd_close_button);
	display_progress_dialog (window);

	fr_archive_progress_cb (NULL, 1.0, window);
	fr_archive_message_cb (NULL, NULL, window);

	basename = _g_file_get_display_basename (window->priv->saving_file);
	/* Translators: %s is a filename */
	description = g_strdup_printf (_("\"%s\" created successfully"), basename);
	progress_dialog_set_action_description (window, description);

	g_free (description);
	g_free (basename);
}