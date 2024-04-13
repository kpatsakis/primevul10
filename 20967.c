_paste_from_archive_operation_completed (FrWindow *window,
					 FrAction  action,
					 GError   *error)
{
	FrArchive *archive;

#ifdef DEBUG
	debug (DEBUG_INFO, "%s [DONE] (FR::Window)\n", action_names[action]);
#endif

	_fr_window_stop_activity_mode (window);
	close_progress_dialog (window, FALSE);

	if ((error != NULL) && (error->code == FR_ERROR_ASK_PASSWORD)) {
		dlg_ask_password_for_second_archive (window);
		return;
	}

	if (action == FR_ACTION_ADDING_FILES)
		archive = window->archive;
	else
		archive = window->priv->copy_from_archive;
	_handle_archive_operation_error (window, archive, action, error, NULL, NULL);

	if (error != NULL) {
		if (window->priv->second_password != NULL) {
			g_free (window->priv->second_password);
			window->priv->second_password = NULL;
		}

		fr_clipboard_data_unref (window->priv->clipboard_data);
		window->priv->clipboard_data = NULL;
	}
}