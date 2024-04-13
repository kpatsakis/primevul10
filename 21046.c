fr_archive_start_cb (FrArchive *archive,
		     FrAction   action,
		     FrWindow  *window)
{
	char *description;

	description = get_action_description (window, action, fr_archive_get_file (archive));
	fr_archive_message_cb (archive, description, window);

	g_free (description);
}