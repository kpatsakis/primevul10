extract_data_new (FrWindow    *window,
		  GList       *file_list,
		  GFile       *destination,
		  const char  *base_dir,
		  gboolean     skip_older,
		  FrOverwrite  overwrite,
		  gboolean     junk_paths,
		  gboolean     extract_here,
		  gboolean     ask_to_open_destination)
{
	ExtractData *edata;

	edata = g_new0 (ExtractData, 1);
	edata->window = window;
	edata->file_list = _g_string_list_dup (file_list);
	edata->destination = _g_object_ref (destination);
	edata->skip_older = skip_older;
	edata->overwrite = overwrite;
	edata->junk_paths = junk_paths;
	if (base_dir != NULL)
		edata->base_dir = g_strdup (base_dir);
	edata->ask_to_open_destination = ask_to_open_destination;

	return edata;
}