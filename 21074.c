_archive_operation_started (FrWindow *window,
			    FrAction  action)
{
	GFile *archive;
	char  *message;

	window->priv->action = action;
	_fr_window_start_activity_mode (window);

#ifdef DEBUG
	debug (DEBUG_INFO, "%s [START] (FR::Window)\n", action_names[action]);
#endif

	archive = window->priv->pd_last_archive;
	if (archive == NULL)
		archive =  window->priv->archive_file;
	message = get_action_description (window, action, archive);
	fr_window_push_message (window, message);
	g_free (message);

	switch (action) {
	case FR_ACTION_EXTRACTING_FILES:
		open_progress_dialog (window, ((window->priv->saving_file != NULL)
					       || window->priv->batch_mode));
		break;
	default:
		open_progress_dialog (window, window->priv->batch_mode);
		break;
	}

	fr_archive_progress_cb (NULL, -1.0, window);
	fr_archive_message_cb (NULL, _("Please wait…"), window);
}