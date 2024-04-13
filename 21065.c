fr_window_archive_save_as (FrWindow   *window,
			   GFile      *file,
			   const char *mime_type,
			   const char *password,
			   gboolean    encrypt_header,
			   guint       volume_size)
{
	FrArchive   *new_archive;
	ConvertData *cdata;

	g_return_if_fail (window != NULL);
	g_return_if_fail (file != NULL);
	g_return_if_fail (window->archive != NULL);

	/* create the new archive */

	new_archive = fr_archive_create (file, mime_type);
	if (new_archive == NULL) {
		GtkWidget *d;
		char      *utf8_name;
		char      *message;

		utf8_name = _g_file_get_display_basename (file);
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

		return;
	}

	cdata = convert_data_new (file, mime_type, password, encrypt_header, volume_size);
	cdata->window = window;
	cdata->new_archive = new_archive;

	_archive_operation_started (window, FR_ACTION_CREATING_ARCHIVE);
	fr_window_set_current_batch_action (window,
					    FR_BATCH_ACTION_SAVE_AS,
					    cdata,
					    (GFreeFunc) convert_data_free);

	g_signal_connect (cdata->new_archive,
			  "progress",
			  G_CALLBACK (fr_archive_progress_cb),
			  window);
	g_signal_connect (cdata->new_archive,
			  "message",
			  G_CALLBACK (fr_archive_message_cb),
			  window);
	g_signal_connect (cdata->new_archive,
			  "start",
			  G_CALLBACK (fr_archive_start_cb),
			  window);
	g_signal_connect (cdata->new_archive,
			  "stoppable",
			  G_CALLBACK (fr_archive_stoppable_cb),
			  window);
	g_signal_connect (cdata->new_archive,
			  "working-archive",
			  G_CALLBACK (fr_window_working_archive_cb),
			  window);

	_g_object_unref (window->priv->saving_file);
	window->priv->saving_file = g_object_ref (cdata->file);

	fr_archive_action_started (window->archive, FR_ACTION_EXTRACTING_FILES);
	fr_archive_extract (window->archive,
			    NULL,
			    cdata->temp_extraction_dir,
			    NULL,
			    FALSE,
			    TRUE,
			    FALSE,
			    window->priv->password,
			    window->priv->cancellable,
			    archive_extraction_ready_for_convertion_cb,
			    cdata);
}