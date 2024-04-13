open_files_data_unref (OpenFilesData *odata)
{
	g_return_if_fail (odata != NULL);

	if (--odata->ref_count > 0)
		return;

	_g_string_list_free (odata->file_list);
	g_free (odata);
}