convert_data_new (GFile      *file,
		  const char *mime_type,
		  const char *password,
		  gboolean    encrypt_header,
	  	  guint       volume_size)
{
	ConvertData *cdata;

	cdata = g_new0 (ConvertData, 1);
	cdata->file = _g_object_ref (file);
	if (mime_type != NULL)
		cdata->mime_type = g_strdup (mime_type);
	if (password != NULL)
		cdata->password = g_strdup (password);
	cdata->encrypt_header = encrypt_header;
	cdata->volume_size = volume_size;
	cdata->temp_extraction_dir = _g_file_get_temp_work_dir (NULL);

	return cdata;
}