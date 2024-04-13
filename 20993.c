_handle_archive_operation_error (FrWindow  *window,
				 FrArchive *archive,
				 FrAction   action,
				 GError    *error,
				 gboolean  *continue_batch,
				 gboolean  *opens_dialog)
{
	GtkWindow *dialog_parent;
	char      *msg;
	char      *utf8_name;
	char      *details;
	GList     *output;
	GtkWidget *dialog;

	if (continue_batch) *continue_batch = (error == NULL);
	if (opens_dialog) *opens_dialog = FALSE;

	if (error == NULL)
		return;

	if ((error != NULL) && (error->code == FR_ERROR_STOPPED))
		g_cancellable_reset (window->priv->cancellable);

	switch (error->code) {
	case FR_ERROR_ASK_PASSWORD:
		close_progress_dialog (window, TRUE);
		dlg_ask_password (window);
		if (opens_dialog) *opens_dialog = TRUE;
		break;

	case FR_ERROR_UNSUPPORTED_FORMAT:
		close_progress_dialog (window, TRUE);
		dlg_package_installer (window,
				       window->priv->archive_file,
				       action,
				       window->priv->cancellable);
		if (opens_dialog) *opens_dialog = TRUE;
		break;

#if 0
	case FR_PROC_ERROR_BAD_CHARSET:
		close_progress_dialog (window, TRUE);
		/* dlg_ask_archive_charset (window); FIXME: implement after feature freeze */
		break;
#endif

	case FR_ERROR_STOPPED:
		/* nothing */
		break;

	default:
		/* generic error => show an error dialog */

		msg = NULL;
		details = NULL;
		output = NULL;

		if (window->priv->batch_mode) {
			dialog_parent = NULL;
			window->priv->load_error_parent_window = NULL;
		}
		else {
			dialog_parent = (GtkWindow *) window;
			if (window->priv->load_error_parent_window == NULL)
				window->priv->load_error_parent_window = (GtkWindow *) window;
		}

		switch (action) {
		case FR_ACTION_CREATING_NEW_ARCHIVE:
			dialog_parent = window->priv->load_error_parent_window;
			msg = _("Could not create the archive");
			break;

		case FR_ACTION_EXTRACTING_FILES:
		case FR_ACTION_COPYING_FILES_TO_REMOTE:
			msg = _("An error occurred while extracting files.");
			break;

		case FR_ACTION_LOADING_ARCHIVE:
			dialog_parent = window->priv->load_error_parent_window;
			utf8_name = _g_file_get_display_basename (window->priv->archive_file);
			msg = g_strdup_printf (_("Could not open \"%s\""), utf8_name);
			g_free (utf8_name);
			break;

		case FR_ACTION_LISTING_CONTENT:
			msg = _("An error occurred while loading the archive.");
			break;

		case FR_ACTION_DELETING_FILES:
			msg = _("An error occurred while deleting files from the archive.");
			break;

		case FR_ACTION_ADDING_FILES:
		case FR_ACTION_GETTING_FILE_LIST:
		case FR_ACTION_COPYING_FILES_FROM_REMOTE:
			msg = _("An error occurred while adding files to the archive.");
			break;

		case FR_ACTION_TESTING_ARCHIVE:
			msg = _("An error occurred while testing archive.");
			break;

		case FR_ACTION_SAVING_REMOTE_ARCHIVE:
		case FR_ACTION_ENCRYPTING_ARCHIVE:
			msg = _("An error occurred while saving the archive.");
			break;

		case FR_ACTION_RENAMING_FILES:
			msg = _("An error occurred while renaming the files.");
			break;

		case FR_ACTION_UPDATING_FILES:
			msg = _("An error occurred while updating the files.");
			break;

		default:
			msg = _("An error occurred.");
			break;
		}

		switch (error->code) {
		case FR_ERROR_COMMAND_NOT_FOUND:
			details = _("Command not found.");
			break;
		case FR_ERROR_EXITED_ABNORMALLY:
			details = _("Command exited abnormally.");
			break;
		case FR_ERROR_SPAWN:
			details = error->message;
			break;
		default:
			details = error->message;
			break;
		}

		if ((error->code != FR_ERROR_GENERIC) && FR_IS_COMMAND (archive))
			output = fr_command_get_last_output (FR_COMMAND (archive));

		dialog = _gtk_error_dialog_new (dialog_parent,
						0,
						output,
						msg,
						((details != NULL) ? "%s" : NULL),
						details);
		fr_window_show_error_dialog (window, dialog, dialog_parent, details);
		break;
	}
}