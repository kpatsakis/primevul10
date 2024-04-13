encrypt_data_new (FrWindow   *window,
		  const char *password,
		  gboolean    encrypt_header)
{
	EncryptData *edata;

	edata = g_new0 (EncryptData, 1);
	edata->window = window;
	if (password != NULL)
		edata->password = g_strdup (password);
	edata->encrypt_header = encrypt_header;
	edata->temp_extraction_dir = _g_file_get_temp_work_dir (NULL);

	return edata;
}