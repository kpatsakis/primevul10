_remove_files_entry_action (SaveData             *save_data,
			    struct archive_entry *w_entry,
			    gpointer              user_data)
{
	RemoveData  *remove_data = user_data;
	LoadData    *load_data = LOAD_DATA (save_data);
	WriteAction  action;
	const char  *pathname;

	action = WRITE_ACTION_WRITE_ENTRY;
	pathname = archive_entry_pathname (w_entry);
	if (g_hash_table_lookup (remove_data->files_to_remove, pathname)) {
		action = WRITE_ACTION_SKIP_ENTRY;
		remove_data->n_files_to_remove--;
		fr_archive_progress_inc_completed_files (load_data->archive, 1);
		g_hash_table_remove (remove_data->files_to_remove, pathname);
	}

	return action;
}