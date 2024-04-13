fr_window_exec_batch_action (FrWindow      *window,
			     FrBatchAction *action)
{
	ExtractData   *edata;
	RenameData    *rdata;
	OpenFilesData *odata;
	ConvertData   *cdata;
	EncryptData   *enc_data;

	switch (action->type) {
	case FR_BATCH_ACTION_LOAD:
		debug (DEBUG_INFO, "[BATCH] LOAD\n");

		if (! g_file_query_exists (G_FILE (action->data), NULL)) {
			GError *error = NULL;

			if (! fr_window_archive_new (window, G_FILE (action->data), NULL))
				error = g_error_new_literal (FR_ERROR, FR_ERROR_GENERIC, _("Archive type not supported."));
			_archive_operation_completed (window, FR_ACTION_CREATING_NEW_ARCHIVE, error);

			_g_error_free (error);
		}
		else
			fr_window_archive_open (window, G_FILE (action->data), GTK_WINDOW (window));
		break;

	case FR_BATCH_ACTION_ADD:
		debug (DEBUG_INFO, "[BATCH] ADD\n");

		fr_window_archive_add_dropped_items (window, (GList *) action->data);
		break;

	case FR_BATCH_ACTION_OPEN:
		debug (DEBUG_INFO, "[BATCH] OPEN\n");

		fr_window_push_message (window, _("Add files to an archive"));
		dlg_batch_add_files (window, (GList *) action->data);
		break;

	case FR_BATCH_ACTION_EXTRACT:
		debug (DEBUG_INFO, "[BATCH] EXTRACT\n");

		edata = action->data;
		fr_window_archive_extract (window,
					   edata->file_list,
					   edata->destination,
					   edata->base_dir,
					   edata->skip_older,
					   edata->overwrite,
					   edata->junk_paths,
					   ! window->priv->batch_mode || window->priv->notify);
		break;

	case FR_BATCH_ACTION_EXTRACT_HERE:
		debug (DEBUG_INFO, "[BATCH] EXTRACT HERE\n");

		edata = action->data;
		fr_window_archive_extract_here (window,
						FALSE,
						TRUE,
						FALSE,
						! window->priv->batch_mode || window->priv->notify);
		break;

	case FR_BATCH_ACTION_EXTRACT_INTERACT:
		debug (DEBUG_INFO, "[BATCH] EXTRACT_INTERACT\n");

		if (window->priv->extract_interact_use_default_dir
		    && (window->priv->extract_default_dir != NULL))
		{
			fr_window_archive_extract (window,
						   NULL,
						   window->priv->extract_default_dir,
						   NULL,
						   FALSE,
						   FR_OVERWRITE_ASK,
						   FALSE,
						   TRUE);
		}
		else {
			fr_window_push_message (window, _("Extract archive"));
			dlg_extract (NULL, window);
		}
		break;

	case FR_BATCH_ACTION_RENAME:
		debug (DEBUG_INFO, "[BATCH] RENAME\n");

		rdata = action->data;
		rename_selection (window,
				  rdata->path_to_rename,
				  rdata->old_name,
				  rdata->new_name,
				  rdata->current_dir,
				  rdata->is_dir,
				  rdata->dir_in_archive,
				  rdata->original_path);
		break;

	case FR_BATCH_ACTION_PASTE:
		debug (DEBUG_INFO, "[BATCH] PASTE\n");

		fr_window_paste_from_clipboard_data (window, (FrClipboardData*) action->data);
		break;

	case FR_BATCH_ACTION_OPEN_FILES:
		debug (DEBUG_INFO, "[BATCH] OPEN FILES\n");

		odata = action->data;
		fr_window_open_files (window, odata->file_list, odata->ask_application);
		break;

	case FR_BATCH_ACTION_SAVE_AS:
		debug (DEBUG_INFO, "[BATCH] SAVE_AS\n");

		cdata = action->data;
		fr_window_archive_save_as (window,
					   cdata->file,
					   cdata->mime_type,
					   cdata->password,
					   cdata->encrypt_header,
					   cdata->volume_size);
		break;

	case FR_BATCH_ACTION_TEST:
		debug (DEBUG_INFO, "[BATCH] TEST\n");

		fr_window_archive_test (window);
		break;

	case FR_BATCH_ACTION_ENCRYPT:
		debug (DEBUG_INFO, "[BATCH] ENCRYPT\n");

		enc_data = action->data;
		fr_window_archive_encrypt (window,
					   enc_data->password,
					   enc_data->encrypt_header);
		break;


	case FR_BATCH_ACTION_CLOSE:
		debug (DEBUG_INFO, "[BATCH] CLOSE\n");

		fr_window_archive_close (window);
		fr_window_exec_next_batch_action (window);
		break;

	case FR_BATCH_ACTION_QUIT:
		debug (DEBUG_INFO, "[BATCH] QUIT\n");

		g_signal_emit (window,
			       fr_window_signals[READY],
			       0,
			       NULL);

		if ((window->priv->progress_dialog != NULL) && (gtk_widget_get_parent (window->priv->progress_dialog) != GTK_WIDGET (window))) {
			gtk_widget_destroy (window->priv->progress_dialog);
			window->priv->progress_dialog = NULL;
		}
		gtk_widget_destroy (GTK_WIDGET (window));
		break;

	default:
		break;
	}
}