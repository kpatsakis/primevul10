update_entry_async_data_free (UpdateEntryAsyncData *data)
{
	if (data->domain != NULL)
		g_object_unref (data->domain);

	if (data->entry != NULL)
		g_object_unref (data->entry);

	g_slice_free (UpdateEntryAsyncData, data);
}