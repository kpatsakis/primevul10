query_info_ready_for_overwrite_dialog_cb (GObject      *source_object,
					  GAsyncResult *result,
					  gpointer      user_data)
{
	OverwriteData *odata = user_data;
	GFile         *destination = G_FILE (source_object);
	GFileInfo     *info;
	GFileType      file_type;

	info = g_file_query_info_finish (destination, result, NULL);
	if (info == NULL) {
		odata->current_file = odata->current_file->next;
		_fr_window_ask_overwrite_dialog (odata);
		return;
	}

	file_type = g_file_info_get_file_type (info);
	if ((file_type != G_FILE_TYPE_UNKNOWN) && (file_type != G_FILE_TYPE_DIRECTORY)) {
		char      *msg;
		GFile     *parent;
		char      *parent_name;
		char      *details;
		GtkWidget *d;

		msg = g_strdup_printf (_("Replace file \"%s\"?"), g_file_info_get_display_name (info));
		parent = g_file_get_parent (destination);
		parent_name = g_file_get_parse_name (parent);
		details = g_strdup_printf (_("Another file with the same name already exists in \"%s\"."), parent_name);
		d = _gtk_message_dialog_new (GTK_WINDOW (odata->window),
					     GTK_DIALOG_MODAL,
					     GTK_STOCK_DIALOG_QUESTION,
					     msg,
					     details,
					     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					     _("Replace _All"), _FR_RESPONSE_OVERWRITE_YES_ALL,
					     _("_Skip"), _FR_RESPONSE_OVERWRITE_NO,
					     _("_Replace"), _FR_RESPONSE_OVERWRITE_YES,
					     NULL);
		gtk_dialog_set_default_response (GTK_DIALOG (d), _FR_RESPONSE_OVERWRITE_YES);
		g_signal_connect (d,
				  "response",
				  G_CALLBACK (overwrite_dialog_response_cb),
				  odata);
		gtk_widget_show (d);

		g_free (parent_name);
		g_object_unref (parent);
		g_object_unref (info);

		return;
	}

	g_object_unref (info);

	odata->current_file = odata->current_file->next;
	_fr_window_ask_overwrite_dialog (odata);
}