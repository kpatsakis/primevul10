encryption_copy_progress_cb (goffset  current_num_bytes,
			     goffset  total_num_bytes,
			     gpointer user_data)
{
	EncryptData *edata = user_data;

	fr_archive_progress (edata->new_archive, (double) current_num_bytes / total_num_bytes);
}