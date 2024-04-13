fr_window_file_list_drag_data_get (FrWindow         *window,
				   GdkDragContext   *context,
				   GtkSelectionData *selection_data,
				   GList            *path_list)
{
	char  *uri;
	GFile *destination;
	GFile *destination_folder;

	debug (DEBUG_INFO, "::DragDataGet -->\n");

	if (window->priv->path_clicked != NULL) {
		gtk_tree_path_free (window->priv->path_clicked);
		window->priv->path_clicked = NULL;
	}

	if (window->priv->activity_ref > 0)
		return FALSE;

	if (gtk_selection_data_get_target (selection_data) == XFR_ATOM) {
		FrClipboardData *tmp;
		char            *data;

		tmp = fr_clipboard_data_new ();
		tmp->files = fr_window_get_file_list_selection (window, TRUE, NULL);
		tmp->op = FR_CLIPBOARD_OP_COPY;
		tmp->base_dir = g_strdup (fr_window_get_current_location (window));

		data = get_selection_data_from_clipboard_data (window, tmp);
		gtk_selection_data_set (selection_data, XFR_ATOM, 8, (guchar *) data, strlen (data));

		fr_clipboard_data_unref (tmp);
		g_free (data);

		return TRUE;
	}

	if (! nautilus_xds_dnd_is_valid_xds_context (context))
		return FALSE;

	uri = get_xds_atom_value (context);
	g_return_val_if_fail (uri != NULL, FALSE);

	destination = g_file_new_for_uri (uri);
	destination_folder = g_file_get_parent (destination);

	g_object_unref (destination);

	/* check whether the extraction can be performed in the destination
	 * folder */

	g_clear_error (&window->priv->drag_error);

	if (! _g_file_check_permissions (destination_folder, R_OK | W_OK)) {
		char *display_name;

		display_name = _g_file_get_display_basename (destination_folder);
		window->priv->drag_error = g_error_new (FR_ERROR, 0, _("You don't have the right permissions to extract archives in the folder \"%s\""), display_name);

		g_free (display_name);
	}

	if (window->priv->drag_error == NULL) {
		_g_object_unref (window->priv->drag_destination_folder);
		g_free (window->priv->drag_base_dir);
		_g_string_list_free (window->priv->drag_file_list);
		window->priv->drag_destination_folder = g_object_ref (destination_folder);
		window->priv->drag_base_dir = g_strdup (fr_window_get_current_location (window));
		window->priv->drag_file_list = fr_window_get_file_list_from_path_list (window, path_list, NULL);
	}

	g_object_unref (destination_folder);

	/* sends back the response */

	gtk_selection_data_set (selection_data, gtk_selection_data_get_target (selection_data), 8, (guchar *) ((window->priv->drag_error == NULL) ? "S" : "E"), 1);

	debug (DEBUG_INFO, "::DragDataGet <--\n");

	return TRUE;
}