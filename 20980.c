fr_window_archive_encrypt (FrWindow   *window,
			   const char *password,
			   gboolean    encrypt_header)
{
	EncryptData *edata;
	GFile       *temp_destination_parent;
	GFile       *temp_destination;
	char        *basename;
	GFile       *temp_new_file;
	FrArchive   *new_archive;

	/* create the new archive */

	if (g_file_is_native (fr_archive_get_file (window->archive)))
		temp_destination_parent = g_file_get_parent (fr_archive_get_file (window->archive));
	else
		temp_destination_parent = NULL;
	temp_destination = _g_file_get_temp_work_dir (temp_destination_parent);
	basename = g_file_get_basename (fr_archive_get_file (window->archive));
	temp_new_file = g_file_get_child (temp_destination, basename);

	g_free (basename);
	_g_object_unref (temp_destination_parent);

	new_archive = fr_archive_create (temp_new_file, fr_archive_get_mime_type (window->archive));
	if (new_archive == NULL) {
		GtkWidget *d;
		char      *utf8_name;
		char      *message;

		utf8_name = _g_file_get_display_basename (temp_new_file);
		message = g_strdup_printf (_("Could not save the archive \"%s\""), utf8_name);
		g_free (utf8_name);

		d = _gtk_error_dialog_new (GTK_WINDOW (window),
					   GTK_DIALOG_DESTROY_WITH_PARENT,
					   NULL,
					   message,
					   "%s",
					   _("Archive type not supported."));
		gtk_dialog_run (GTK_DIALOG (d));
		gtk_widget_destroy (d);

		g_free (message);
		g_object_unref (temp_new_file);

		_g_file_remove_directory (temp_destination, NULL, NULL);
		g_object_unref (temp_destination);

		return;
	}

	g_object_unref (temp_destination);

	edata = encrypt_data_new (window, password, encrypt_header);
	edata->temp_new_file = temp_new_file;
	edata->new_archive = new_archive;

	g_signal_connect (edata->new_archive,
			  "progress",
			  G_CALLBACK (fr_archive_progress_cb),
			  window);
	g_signal_connect (edata->new_archive,
			  "message",
			  G_CALLBACK (fr_archive_message_cb),
			  window);
	g_signal_connect (edata->new_archive,
			  "start",
			  G_CALLBACK (fr_archive_start_cb),
			  window);
	g_signal_connect (edata->new_archive,
			  "stoppable",
			  G_CALLBACK (fr_archive_stoppable_cb),
			  window);
	g_signal_connect (edata->new_archive,
			  "working-archive",
			  G_CALLBACK (fr_window_working_archive_cb),
			  window);

	_archive_operation_started (window, FR_ACTION_ENCRYPTING_ARCHIVE);
	fr_window_set_current_batch_action (window,
					    FR_BATCH_ACTION_ENCRYPT,
					    edata,
					    (GFreeFunc) encrypt_data_free);

	fr_archive_action_started (window->archive, FR_ACTION_EXTRACTING_FILES);
	fr_archive_extract (window->archive,
			    NULL,
			    edata->temp_extraction_dir,
			    NULL,
			    FALSE,
			    TRUE,
			    FALSE,
			    window->priv->password,
			    window->priv->cancellable,
			    archive_extraction_ready_for_encryption_cb,
			    edata);
}