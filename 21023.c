fr_window_drag_data_received  (GtkWidget          *widget,
			       GdkDragContext     *context,
			       gint                x,
			       gint                y,
			       GtkSelectionData   *data,
			       guint               info,
			       guint               time,
			       gpointer            extra_data)
{
	FrWindow  *window = extra_data;
	GList     *list;
	gboolean   one_file;
	gboolean   is_an_archive;

	debug (DEBUG_INFO, "::DragDataReceived -->\n");

	if ((gtk_drag_get_source_widget (context) == window->priv->list_view)
	    || (gtk_drag_get_source_widget (context) == window->priv->tree_view))
	{
		gtk_drag_finish (context, FALSE, FALSE, time);
		return;
	}

	if (! ((gtk_selection_data_get_length (data) >= 0) && (gtk_selection_data_get_format (data) == 8))) {
		gtk_drag_finish (context, FALSE, FALSE, time);
		return;
	}

	if (window->priv->activity_ref > 0) {
		gtk_drag_finish (context, FALSE, FALSE, time);
		return;
	}

	gtk_drag_finish (context, TRUE, FALSE, time);

	if (gtk_selection_data_get_target (data) == XFR_ATOM) {
		FrClipboardData *dnd_data;

		dnd_data = get_clipboard_data_from_selection_data (window, (char*) gtk_selection_data_get_data (data));
		dnd_data->current_dir = g_strdup (fr_window_get_current_location (window));
		fr_window_paste_from_clipboard_data (window, dnd_data);

		return;
	}

	list = get_file_list_from_selection_data ((char*) gtk_selection_data_get_data (data));
	if (list == NULL) {
		GtkWidget *d;

		d = _gtk_error_dialog_new (GTK_WINDOW (window),
					   GTK_DIALOG_MODAL,
					   NULL,
					   _("Could not perform the operation"),
					   NULL);
		gtk_dialog_run (GTK_DIALOG (d));
		gtk_widget_destroy(d);

 		return;
	}

	one_file = (list->next == NULL);
	if (one_file)
		is_an_archive = _g_file_is_archive (G_FILE (list->data));
	else
		is_an_archive = FALSE;

	if (window->priv->archive_present
	    && (window->archive != NULL)
	    && ! window->archive->read_only
	    && fr_archive_is_capable_of (window->archive, FR_ARCHIVE_CAN_STORE_MANY_FILES))
	{
		if (one_file && is_an_archive) {
			GtkWidget *d;
			gint       r;

			d = _gtk_message_dialog_new (GTK_WINDOW (window),
						     GTK_DIALOG_MODAL,
						     GTK_STOCK_DIALOG_QUESTION,
						     _("Do you want to add this file to the current archive or open it as a new archive?"),
						     NULL,
						     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						     GTK_STOCK_ADD, 0,
						     GTK_STOCK_OPEN, 1,
						     NULL);

			gtk_dialog_set_default_response (GTK_DIALOG (d), 2);

			r = gtk_dialog_run (GTK_DIALOG (d));
			gtk_widget_destroy (GTK_WIDGET (d));

			if (r == 0)  /* Add */
				fr_window_archive_add_dropped_items (window, list);
			else if (r == 1)  /* Open */
				fr_window_archive_open (window, G_FILE (list->data), GTK_WINDOW (window));
 		}
 		else
			fr_window_archive_add_dropped_items (window, list);
	}
	else {
		if (one_file && is_an_archive)
			fr_window_archive_open (window, G_FILE (list->data), GTK_WINDOW (window));
		else {
			GtkWidget *d;
			int        r;

			d = _gtk_message_dialog_new (GTK_WINDOW (window),
						     GTK_DIALOG_MODAL,
						     GTK_STOCK_DIALOG_QUESTION,
						     _("Do you want to create a new archive with these files?"),
						     NULL,
						     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						     _("Create _Archive"), GTK_RESPONSE_YES,
						     NULL);

			gtk_dialog_set_default_response (GTK_DIALOG (d), GTK_RESPONSE_YES);
			r = gtk_dialog_run (GTK_DIALOG (d));
			gtk_widget_destroy (GTK_WIDGET (d));

			if (r == GTK_RESPONSE_YES) {
				GFile     *first_file;
				GFile     *folder;
				char      *archive_name;
				GtkWidget *dialog;

				fr_window_free_batch_data (window);
				fr_window_append_batch_action (window,
							       FR_BATCH_ACTION_ADD,
							       _g_object_list_ref (list),
							       (GFreeFunc) _g_object_list_unref);

				first_file = G_FILE (list->data);
				folder = g_file_get_parent (first_file);
				if (folder != NULL)
					fr_window_set_open_default_dir (window, folder);

				if ((list->next != NULL) && (folder != NULL))
					archive_name = g_file_get_basename (folder);
				else
					archive_name = g_file_get_basename (first_file);

				dialog = fr_new_archive_dialog_new (_("New Archive"),
								    GTK_WINDOW (window),
								    FR_NEW_ARCHIVE_ACTION_SAVE_AS,
								    fr_window_get_open_default_dir (window),
								    archive_name,
								    NULL);
				gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
				g_signal_connect (G_OBJECT (dialog),
						  "response",
						  G_CALLBACK (new_archive_dialog_response_cb),
						  window);
				gtk_window_present (GTK_WINDOW (dialog));

				g_free (archive_name);
				_g_object_unref (folder);
			}
		}
	}

	_g_object_list_unref (list);

	debug (DEBUG_INFO, "::DragDataReceived <--\n");
}