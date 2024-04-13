fr_window_archive_extract (FrWindow    *window,
			   GList       *file_list,
			   GFile       *destination,
			   const char  *base_dir,
			   gboolean     skip_older,
			   FrOverwrite  overwrite,
			   gboolean     junk_paths,
			   gboolean     ask_to_open_destination)
{
	ExtractData *edata;
	gboolean     do_not_extract = FALSE;
	GError      *error = NULL;

	edata = extract_data_new (window,
				  file_list,
				  destination,
				  base_dir,
				  skip_older,
				  overwrite,
				  junk_paths,
				  FALSE,
				  ask_to_open_destination);

	fr_window_set_current_batch_action (window,
					    FR_BATCH_ACTION_EXTRACT,
					    edata,
					    (GFreeFunc) extract_data_free);

	if (archive_is_encrypted (window, edata->file_list) && (window->priv->password == NULL)) {
		dlg_ask_password (window);
		return;
	}

	if (! _g_file_query_is_dir (edata->destination)) {

		/* There is nothing to ask if the destination doesn't exist. */
		if (edata->overwrite == FR_OVERWRITE_ASK)
			edata->overwrite = FR_OVERWRITE_YES;

		if (! ForceDirectoryCreation) {
			GtkWidget *d;
			int        r;
			char      *folder_name;
			char      *msg;

			folder_name = _g_file_get_display_basename (edata->destination);
			msg = g_strdup_printf (_("Destination folder \"%s\" does not exist.\n\nDo you want to create it?"), folder_name);
			g_free (folder_name);

			d = _gtk_message_dialog_new (GTK_WINDOW (window),
						     GTK_DIALOG_MODAL,
						     GTK_STOCK_DIALOG_QUESTION,
						     msg,
						     NULL,
						     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						     _("Create _Folder"), GTK_RESPONSE_YES,
						     NULL);

			gtk_dialog_set_default_response (GTK_DIALOG (d), GTK_RESPONSE_YES);
			r = gtk_dialog_run (GTK_DIALOG (d));
			gtk_widget_destroy (GTK_WIDGET (d));

			g_free (msg);

			if (r != GTK_RESPONSE_YES)
				do_not_extract = TRUE;
		}

		if (! do_not_extract && ! _g_file_make_directory_tree (edata->destination, 0755, &error)) {
			GtkWidget *d;
			char      *details;

			details = g_strdup_printf (_("Could not create the destination folder: %s."), error->message);
			d = _gtk_error_dialog_new (GTK_WINDOW (window),
						   0,
						   NULL,
						   _("Extraction not performed"),
						   "%s",
						   details);
			g_clear_error (&error);
			fr_window_show_error_dialog (window, d, GTK_WINDOW (window), details);
			fr_window_stop_batch (window);

			g_free (details);

			return;
		}
	}

	if (do_not_extract) {
		GtkWidget *d;

		d = _gtk_message_dialog_new (GTK_WINDOW (window),
					     0,
					     GTK_STOCK_DIALOG_WARNING,
					     _("Extraction not performed"),
					     NULL,
					     GTK_STOCK_OK, GTK_RESPONSE_OK,
					     NULL);
		gtk_dialog_set_default_response (GTK_DIALOG (d), GTK_RESPONSE_OK);
		fr_window_show_error_dialog (window, d, GTK_WINDOW (window), _("Extraction not performed"));
		fr_window_stop_batch (window);

		return;
	}

	if (edata->overwrite == FR_OVERWRITE_ASK) {
		OverwriteData *odata;

		odata = g_new0 (OverwriteData, 1);
		odata->window = window;
		odata->edata = edata;
		odata->extract_all = (edata->file_list == NULL) || (g_list_length (edata->file_list) == window->archive->files->len);
		if (edata->file_list == NULL)
			edata->file_list = fr_window_get_file_list (window);
		odata->current_file = odata->edata->file_list;

		_fr_window_ask_overwrite_dialog (odata);
	}
	else
		_fr_window_archive_extract_from_edata (window, edata);
}