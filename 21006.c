overwrite_dialog_response_cb (GtkDialog *dialog,
			      int        response_id,
			      gpointer   user_data)
{
	OverwriteData *odata = user_data;
	gboolean       do_not_extract = FALSE;

	switch (response_id) {
	case _FR_RESPONSE_OVERWRITE_YES_ALL:
		odata->edata->overwrite = FR_OVERWRITE_YES;
		break;

	case _FR_RESPONSE_OVERWRITE_YES:
		odata->current_file = odata->current_file->next;
		break;

	case _FR_RESPONSE_OVERWRITE_NO:
		{
			/* remove the file from the list to extract */
			GList *next = odata->current_file->next;

			odata->edata->file_list = g_list_remove_link (odata->edata->file_list, odata->current_file);
			_g_string_list_free (odata->current_file);
			odata->current_file = next;
			odata->extract_all = FALSE;
		}
		break;

	case GTK_RESPONSE_DELETE_EVENT:
	case GTK_RESPONSE_CANCEL:
		do_not_extract = TRUE;
		break;

	default:
		break;
	}

	gtk_widget_destroy (GTK_WIDGET (dialog));

	if (do_not_extract) {
		fr_window_stop_batch (odata->window);
		g_free (odata);
		return;
	}

	_fr_window_ask_overwrite_dialog (odata);
}