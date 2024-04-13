get_clipboard_data_from_selection_data (FrWindow   *window,
					const char *data)
{
	FrClipboardData  *clipboard_data;
	char            **uris;
	int               i;

	clipboard_data = fr_clipboard_data_new ();

	uris = g_strsplit (data, "\r\n", -1);

	clipboard_data->file = g_file_new_for_uri (uris[0]);
	if (window->priv->second_password != NULL)
		clipboard_data->password = g_strdup (window->priv->second_password);
	else if (strcmp (uris[1], "") != 0)
		clipboard_data->password = g_strdup (uris[1]);
	clipboard_data->op = (strcmp (uris[2], "copy") == 0) ? FR_CLIPBOARD_OP_COPY : FR_CLIPBOARD_OP_CUT;
	clipboard_data->base_dir = g_strdup (uris[3]);
	for (i = 4; uris[i] != NULL; i++)
		if (uris[i][0] != '\0')
			clipboard_data->files = g_list_prepend (clipboard_data->files, g_strdup (uris[i]));
	clipboard_data->files = g_list_reverse (clipboard_data->files);

	g_strfreev (uris);

	return clipboard_data;
}