_remove_files_begin (SaveData *save_data,
		     gpointer  user_data)
{
	LoadData   *load_data = LOAD_DATA (save_data);
	RemoveData *remove_data = user_data;

	fr_archive_progress_set_total_files (load_data->archive, remove_data->n_files_to_remove);
	fr_archive_progress_set_total_bytes (load_data->archive,
				FR_ARCHIVE_LIBARCHIVE (load_data->archive)->priv->uncompressed_size);
}