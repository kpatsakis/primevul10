get_action_description (FrWindow *window,
			FrAction  action,
			GFile    *file)
{
	char *basename;
	char *message;

	basename = _g_file_get_display_basename (file);

	message = NULL;
	switch (action) {
	case FR_ACTION_CREATING_NEW_ARCHIVE:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Creating \"%s\""), basename);
		break;
	case FR_ACTION_LOADING_ARCHIVE:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Loading \"%s\""), basename);
		break;
	case FR_ACTION_LISTING_CONTENT:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Reading \"%s\""), basename);
		break;
	case FR_ACTION_DELETING_FILES:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Deleting the files from \"%s\""), basename);
		break;
	case FR_ACTION_TESTING_ARCHIVE:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Testing \"%s\""), basename);
		break;
	case FR_ACTION_GETTING_FILE_LIST:
		message = g_strdup (_("Getting the file list"));
		break;
	case FR_ACTION_COPYING_FILES_FROM_REMOTE:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Copying the files to add to \"%s\""), basename);
		break;
	case FR_ACTION_ADDING_FILES:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Adding the files to \"%s\""), basename);
		break;
	case FR_ACTION_EXTRACTING_FILES:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Extracting the files from \"%s\""), basename);
		break;
	case FR_ACTION_COPYING_FILES_TO_REMOTE:
		message = g_strdup (_("Copying the extracted files to the destination"));
		break;
	case FR_ACTION_CREATING_ARCHIVE:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Creating \"%s\""), basename);
		break;
	case FR_ACTION_SAVING_REMOTE_ARCHIVE:
	case FR_ACTION_ENCRYPTING_ARCHIVE:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Saving \"%s\""), basename);
		break;
	case FR_ACTION_PASTING_FILES:
		message = g_strdup (window->priv->custom_action_message);
		break;
	case FR_ACTION_RENAMING_FILES:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Renaming the files in \"%s\""), basename);
		break;
	case FR_ACTION_UPDATING_FILES:
		/* Translators: %s is a filename */
		message = g_strdup_printf (_("Updating the files in \"%s\""), basename);
		break;
	case FR_ACTION_NONE:
		break;
	}

	g_free (basename);

	return message;
}