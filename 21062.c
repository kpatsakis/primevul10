fr_window_current_folder_activated (FrWindow *window,
				    gboolean   from_sidebar)
{
	char *dir_path;

	if (! from_sidebar) {
		FileData *fdata;
		char     *dir_name;

		fdata = fr_window_get_selected_item_from_file_list (window);
		if ((fdata == NULL) || ! file_data_is_dir (fdata)) {
			file_data_free (fdata);
			return;
		}
		dir_name = g_strdup (fdata->list_name);
		dir_path = g_strconcat (fr_window_get_current_location (window),
					dir_name,
					"/",
					NULL);
		g_free (dir_name);
		file_data_free (fdata);
	}
	else
		dir_path = fr_window_get_selected_folder_in_tree_view (window);

	fr_window_go_to_location (window, dir_path, FALSE);

	g_free (dir_path);
}