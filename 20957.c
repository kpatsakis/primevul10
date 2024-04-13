_fr_window_archive_extract_from_edata (FrWindow    *window,
				       ExtractData *edata)
{
	GList *scan;
	gsize  total_size;

	total_size = 0;
	for (scan = edata->file_list; scan; scan = scan->next) {
		char     *filename = scan->data;
		FileData *file_data;

		file_data = g_hash_table_lookup (window->archive->files_hash, filename);
		if (file_data == NULL)
			continue;

		total_size += file_data->size;
	}
	fr_archive_progress_set_total_bytes (window->archive, total_size);

	_archive_operation_started (window, FR_ACTION_EXTRACTING_FILES);

	fr_archive_extract (window->archive,
			    edata->file_list,
			    edata->destination,
			    edata->base_dir,
			    edata->skip_older,
			    edata->overwrite == FR_OVERWRITE_YES,
			    edata->junk_paths,
			    window->priv->password,
			    window->priv->cancellable,
			    archive_extraction_ready_cb,
			    edata);
}