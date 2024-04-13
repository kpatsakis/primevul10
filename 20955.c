_window_started_loading_file (FrWindow *window,
			      GFile    *file)
{
	char *description;

	description = get_action_description (window, FR_ACTION_LOADING_ARCHIVE, file);
	fr_archive_message_cb (NULL, description, window);

	g_free (description);
}