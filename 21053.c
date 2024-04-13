fr_window_action_save_as (FrWindow *window)
{
	char      *archive_name;
	GtkWidget *dialog;

	archive_name = NULL;
	if (window->priv->archive_file != NULL) {
		GFileInfo *info;

		info = g_file_query_info (window->priv->archive_file,
					  G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
					  0, NULL, NULL);

		if (info != NULL) {
			archive_name = g_strdup (g_file_info_get_display_name (info));
			g_object_unref (info);
		}
	}

	dialog = fr_new_archive_dialog_new (_("Save"),
					    GTK_WINDOW (window),
					    FR_NEW_ARCHIVE_ACTION_SAVE_AS,
					    fr_window_get_open_default_dir (window),
					    archive_name,
					    window->priv->archive_file);
	gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	g_signal_connect (G_OBJECT (dialog),
			  "response",
			  G_CALLBACK (save_as_archive_dialog_response_cb),
			  window);
	gtk_window_present (GTK_WINDOW (dialog));

	g_free (archive_name);
}