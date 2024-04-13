open_progress_dialog_with_open_destination (FrWindow *window)
{
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
	gtk_widget_hide (window->priv->pd_open_archive_button);
	gtk_widget_show (window->priv->pd_open_destination_button);
	gtk_widget_set_visible (window->priv->pd_quit_button, ! window->priv->quit_with_progress_dialog);
	gtk_widget_show (window->priv->pd_close_button);
	display_progress_dialog (window);

	fr_archive_progress_cb (NULL, 1.0, window);
	fr_archive_message_cb (NULL, NULL, window);

	progress_dialog_set_action_description (window, _("Extraction completed successfully"));
}