fr_window_archive_list (FrWindow *window)
{
	if (! fr_archive_is_capable_of (window->archive, FR_ARCHIVE_CAN_READ)) {
		fr_window_archive_close (window);
		fr_window_archive_open (window, window->priv->archive_file, NULL);
		return;
	}

	_archive_operation_started (window, FR_ACTION_LISTING_CONTENT);
	fr_archive_list (window->archive,
			 window->priv->password,
			 window->priv->cancellable,
			 archive_list_ready_cb,
			 window);
}