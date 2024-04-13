open_files_data_ref (OpenFilesData *odata)
{
	g_return_if_fail (odata != NULL);
	odata->ref_count++;
}