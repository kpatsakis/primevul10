fr_window_present_dialog_if_created (FrWindow   *window,
				     const char *dialog_name)
{
	GtkWidget *dialog;

	dialog = g_hash_table_lookup (window->priv->named_dialogs, dialog_name);
	if (dialog != NULL) {
		gtk_window_present (GTK_WINDOW (dialog));
		return TRUE;
	}

	return FALSE;
}