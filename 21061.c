fr_window_clipboard_remove_file_list (FrWindow *window,
				      GList    *file_list)
{
	GList *scan1;

	if (window->priv->copy_data == NULL)
		return;

	if (file_list == NULL) {
		fr_clipboard_data_unref	 (window->priv->copy_data);
		window->priv->copy_data = NULL;
		return;
	}

	for (scan1 = file_list; scan1; scan1 = scan1->next) {
		const char *name1 = scan1->data;
		GList      *scan2;

		for (scan2 = window->priv->copy_data->files; scan2;) {
			const char *name2 = scan2->data;

			if (strcmp (name1, name2) == 0) {
				GList *tmp = scan2->next;
				window->priv->copy_data->files = g_list_remove_link (window->priv->copy_data->files, scan2);
				g_free (scan2->data);
				g_list_free (scan2);
				scan2 = tmp;
			}
			else
				scan2 = scan2->next;
		}
	}

	if (window->priv->copy_data->files == NULL) {
		fr_clipboard_data_unref (window->priv->copy_data);
		window->priv->copy_data = NULL;
		gtk_clipboard_clear (gtk_widget_get_clipboard (GTK_WIDGET (window), FR_CLIPBOARD));
	}
}