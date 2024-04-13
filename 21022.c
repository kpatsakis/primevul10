extract_data_free (ExtractData *extract_data)
{
	g_free (extract_data->base_dir);
	_g_object_unref (extract_data->destination);
	_g_string_list_free (extract_data->file_list);
	g_hash_table_unref (extract_data->files_to_extract);
	g_hash_table_unref (extract_data->usernames);
	g_hash_table_unref (extract_data->groupnames);
	load_data_free (LOAD_DATA (extract_data));
}