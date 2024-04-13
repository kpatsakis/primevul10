save_as_archive_dialog_response_cb (GtkDialog *dialog,
				    int        response,
				    gpointer   user_data)
{
	FrWindow   *window = user_data;
	GFile      *file;
	const char *mime_type;
	const char *password;
	gboolean    encrypt_header;
	int         volume_size;
	GSettings  *settings;

	if ((response == GTK_RESPONSE_CANCEL) || (response == GTK_RESPONSE_DELETE_EVENT)) {
		gtk_widget_destroy (GTK_WIDGET (dialog));
		_archive_operation_cancelled (window, FR_ACTION_CREATING_ARCHIVE);
		return;
	}

	if (response != GTK_RESPONSE_OK)
		return;

	file = fr_new_archive_dialog_get_file (FR_NEW_ARCHIVE_DIALOG (dialog), &mime_type);
	if (file == NULL)
		return;

	password = fr_new_archive_dialog_get_password (FR_NEW_ARCHIVE_DIALOG (dialog));
	encrypt_header = fr_new_archive_dialog_get_encrypt_header (FR_NEW_ARCHIVE_DIALOG (dialog));
	volume_size = fr_new_archive_dialog_get_volume_size (FR_NEW_ARCHIVE_DIALOG (dialog));

	settings = g_settings_new (FILE_ROLLER_SCHEMA_NEW);
	g_settings_set_int (settings, PREF_NEW_VOLUME_SIZE, volume_size);
	g_object_unref (settings);

	fr_window_archive_save_as (window, file, mime_type, password, encrypt_header, volume_size);

	gtk_widget_destroy (GTK_WIDGET (dialog));
	g_object_unref (file);
}