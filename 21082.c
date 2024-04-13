get_dir_list_from_file_data (FrWindow *window,
			     FileData *fdata)
{
	char  *dirname;
	GList *list;

	dirname = g_strconcat (fr_window_get_current_location (window),
			       fdata->list_name,
			       NULL);
	list = get_dir_list_from_path (window, dirname);
	g_free (dirname);

	return list;
}