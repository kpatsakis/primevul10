get_selection_data_from_clipboard_data (FrWindow        *window,
		      			FrClipboardData *data)
{
	GString *list;
	char    *uri;
	GList   *scan;

	if (data == NULL)
		return NULL;

	list = g_string_new (NULL);

	uri = g_file_get_uri (fr_archive_get_file (window->archive));
	g_string_append (list, uri);
	g_free (uri);

	g_string_append (list, "\r\n");
	if (window->priv->password != NULL)
		g_string_append (list, window->priv->password);
	g_string_append (list, "\r\n");
	g_string_append (list, (data->op == FR_CLIPBOARD_OP_COPY) ? "copy" : "cut");
	g_string_append (list, "\r\n");
	g_string_append (list, data->base_dir);
	g_string_append (list, "\r\n");
	for (scan = data->files; scan; scan = scan->next) {
		g_string_append (list, scan->data);
		g_string_append (list, "\r\n");
	}

	return g_string_free (list, FALSE);
}