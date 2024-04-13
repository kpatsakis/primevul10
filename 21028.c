_archive_operation_completed (FrWindow *window,
			      FrAction  action,
			      GError   *error)
{
	gboolean  continue_batch = FALSE;
	gboolean  opens_dialog;
	gboolean  operation_canceled;
	GFile    *archive_dir;
	gboolean  is_temp_dir;

#ifdef DEBUG
	debug (DEBUG_INFO, "%s [DONE] (FR::Window)\n", action_names[action]);
#endif

	_fr_window_stop_activity_mode (window);
	_handle_archive_operation_error (window, window->archive, action, error, &continue_batch, &opens_dialog);
	if (opens_dialog)
		return;

	operation_canceled = g_error_matches (error, FR_ERROR, FR_ERROR_STOPPED);

	switch (action) {
	case FR_ACTION_CREATING_NEW_ARCHIVE:
	case FR_ACTION_CREATING_ARCHIVE:
		close_progress_dialog (window, FALSE);
		if (! operation_canceled) {
			fr_window_history_clear (window);
			fr_window_go_to_location (window, "/", TRUE);
			fr_window_update_dir_tree (window);
			fr_window_update_title (window);
			fr_window_update_sensitivity (window);
		}
		break;

	case FR_ACTION_LOADING_ARCHIVE:
		close_progress_dialog (window, FALSE);
		if (error != NULL) {
			fr_window_remove_from_recent_list (window, window->priv->archive_file);
			fr_window_archive_close (window);
		}
		else {
			fr_window_archive_list (window);
			return;
		}
		break;

	case FR_ACTION_LISTING_CONTENT:
		/* update the file because multi-volume archives can have
		 * a different name after loading. */
		_g_object_unref (window->priv->archive_file);
		window->priv->archive_file = _g_object_ref (fr_archive_get_file (window->archive));

		window->priv->reload_archive = FALSE;

		close_progress_dialog (window, FALSE);
		if (error != NULL) {
			fr_window_remove_from_recent_list (window, window->priv->archive_file);
			fr_window_archive_close (window);
			fr_window_set_password (window, NULL);
			break;
		}

		/* error == NULL */

		archive_dir = g_file_get_parent (window->priv->archive_file);
		is_temp_dir = _g_file_is_temp_dir (archive_dir);
		if (! window->priv->archive_present) {
			window->priv->archive_present = TRUE;

			fr_window_history_clear (window);
			fr_window_history_add (window, "/");

			if (! is_temp_dir) {
				fr_window_set_open_default_dir (window, archive_dir);
				fr_window_set_add_default_dir (window, archive_dir);
				if (! window->priv->freeze_default_dir)
					fr_window_set_extract_default_dir (window, archive_dir, FALSE);
			}

			window->priv->archive_new = FALSE;
		}
		g_object_unref (archive_dir);

		if (! is_temp_dir)
			fr_window_add_to_recent_list (window, window->priv->archive_file);

		fr_window_update_history (window);
		fr_window_update_title (window);
		fr_window_go_to_location (window, fr_window_get_current_location (window), TRUE);
		fr_window_update_dir_tree (window);

		if (! window->priv->batch_mode)
			gtk_window_present (GTK_WINDOW (window));
		break;

	case FR_ACTION_DELETING_FILES:
	case FR_ACTION_ADDING_FILES:
		close_progress_dialog (window, FALSE);

		/* update the file because multi-volume archives can have
		 * a different name after creation. */
		_g_object_unref (window->priv->archive_file);
		window->priv->archive_file = _g_object_ref (fr_archive_get_file (window->archive));

		if (window->priv->notify) {
			_g_object_unref (window->priv->saving_file);
			window->priv->saving_file = g_object_ref (window->priv->archive_file);
		}

		if (error == NULL) {
			if (window->priv->archive_new)
				window->priv->archive_new = FALSE;
			fr_window_add_to_recent_list (window, window->priv->archive_file);
		}

		if (! window->priv->batch_mode && ! operation_canceled)
			window->priv->reload_archive = TRUE;

		break;

	case FR_ACTION_TESTING_ARCHIVE:
		close_progress_dialog (window, FALSE);
		if (error == NULL)
			fr_window_view_last_output (window, _("Test Result"));
		return;

	case FR_ACTION_EXTRACTING_FILES:
		close_progress_dialog (window, FALSE);
		break;

	case FR_ACTION_RENAMING_FILES:
	case FR_ACTION_UPDATING_FILES:
		close_progress_dialog (window, FALSE);
		if (! window->priv->batch_mode && ! operation_canceled)
			window->priv->reload_archive = TRUE;
		break;

	default:
		close_progress_dialog (window, FALSE);
		continue_batch = FALSE;
		break;
	}

	if (continue_batch)
		fr_window_exec_next_batch_action (window);
	else
		fr_window_stop_batch (window);
}