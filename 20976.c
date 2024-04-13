fr_clipboard_get (GtkClipboard     *clipboard,
		  GtkSelectionData *selection_data,
		  guint             info,
		  gpointer          user_data_or_owner)
{
	FrWindow *window = user_data_or_owner;
	char     *data;

	if (gtk_selection_data_get_target (selection_data) != FR_SPECIAL_URI_LIST)
		return;

	data = get_selection_data_from_clipboard_data (window, window->priv->copy_data);
	if (data != NULL) {
		gtk_selection_data_set (selection_data,
					gtk_selection_data_get_target (selection_data),
					8,
					(guchar *) data,
					strlen (data));
		g_free (data);
	}
}