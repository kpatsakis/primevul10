open_files_data_new (FrWindow *window,
		     GList    *file_list,
		     gboolean  ask_application)

{
	OpenFilesData *odata;
	GList         *scan;

	odata = g_new0 (OpenFilesData, 1);
	odata->ref_count = 1;
	odata->window = window;
	odata->file_list = _g_string_list_dup (file_list);
	odata->ask_application = ask_application;
	odata->cdata = g_new0 (CommandData, 1);
	odata->cdata->temp_dir = _g_file_get_temp_work_dir (NULL);
	odata->cdata->file_list = NULL;
	for (scan = file_list; scan; scan = scan->next) {
		char  *filename = scan->data;
		GFile *file;

		file = _g_file_append_path (odata->cdata->temp_dir, filename, NULL);
		odata->cdata->file_list = g_list_prepend (odata->cdata->file_list, file);
	}

	/* Add to CommandList so the cdata is released on exit. */
	CommandList = g_list_prepend (CommandList, odata->cdata);

	return odata;
}