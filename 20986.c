archive_is_encrypted (FrWindow *window,
		      GList    *file_list)
{
	gboolean encrypted = FALSE;

	if (file_list == NULL) {
		int i;

		for (i = 0; ! encrypted && i < window->archive->files->len; i++) {
			FileData *fdata = g_ptr_array_index (window->archive->files, i);

			if (fdata->encrypted)
				encrypted = TRUE;
		}
	}
	else {
		GList *scan;

		for (scan = file_list; ! encrypted && scan; scan = scan->next) {
			char     *filename = scan->data;
			FileData *fdata;

			fdata = g_hash_table_lookup (window->archive->files_hash, filename);
			g_return_val_if_fail (fdata != NULL, FALSE);

			if (fdata->encrypted)
				encrypted = TRUE;
		}
	}

	return encrypted;
}