fr_window_action_new_archive (FrWindow *window)
{
	GtkWidget *dialog;

	if (fr_window_present_dialog_if_created (window, "new_archive"))
		return;

	dialog = fr_new_archive_dialog_new (_("New Archive"),
					    GTK_WINDOW (window),
					    FR_NEW_ARCHIVE_ACTION_NEW_MANY_FILES,
					    fr_window_get_open_default_dir (window),
					    NULL,
					    NULL);
	if ((fr_window_archive_is_present (window) && ! fr_window_is_batch_mode (window) ? NULL : GTK_WINDOW (window)))
		gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
	g_signal_connect (G_OBJECT (dialog),
			  "response",
			  G_CALLBACK (new_archive_dialog_response_cb),
			  window);
	fr_window_set_dialog (window, "new_archive", dialog);
	gtk_window_present (GTK_WINDOW (dialog));
}