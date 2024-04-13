archive_list_ready_cb (GObject      *source_object,
		       GAsyncResult *result,
		       gpointer      user_data)
{
	FrWindow *window = user_data;
	GError   *error = NULL;

	fr_archive_operation_finish (FR_ARCHIVE (source_object), result, &error);
	_archive_operation_completed (window, FR_ACTION_LISTING_CONTENT, error);

	_g_error_free (error);
}