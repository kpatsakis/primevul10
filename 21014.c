encrypt_data_free (EncryptData *edata)
{
	if (edata == NULL)
		return;

	if (edata->temp_new_file != NULL) {
		GFile *parent = g_file_get_parent (edata->temp_new_file);
		if (parent != NULL)
			_g_file_remove_directory (parent, NULL, NULL);
		_g_object_unref (parent);
	}
	_g_object_unref (edata->temp_new_file);
	_g_object_unref (edata->new_archive);
	_g_file_remove_directory (edata->temp_extraction_dir, NULL, NULL);
	_g_object_unref (edata->temp_extraction_dir);
	g_free (edata->password);
	g_free (edata);
}