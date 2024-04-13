paste_from_archive_completed_successfully (FrWindow *window)
{
	_paste_from_archive_operation_completed (window, FR_ACTION_PASTING_FILES, NULL);

	fr_clipboard_data_unref (window->priv->clipboard_data);
	window->priv->clipboard_data = NULL;

	if (window->priv->second_password != NULL) {
		g_free (window->priv->second_password);
		window->priv->second_password = NULL;
	}

	window->priv->archive_new = FALSE;
	fr_window_archive_reload (window);
}