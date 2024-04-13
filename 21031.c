convert_data_free (ConvertData *cdata)
{
	if (cdata == NULL)
		return;
	_g_file_remove_directory (cdata->temp_extraction_dir, NULL, NULL);
	_g_object_unref (cdata->temp_extraction_dir);
	_g_object_unref (cdata->file);
	_g_object_unref (cdata->new_archive);
	g_free (cdata->mime_type);
	g_free (cdata->password);
	g_free (cdata);
}