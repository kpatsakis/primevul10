query_single_entry_async_data_free (QuerySingleEntryAsyncData *data)
{
	if (data->domain != NULL)
		g_object_unref (data->domain);

	g_free (data->entry_id);
	if (data->query != NULL)
		g_object_unref (data->query);
	g_slice_free (QuerySingleEntryAsyncData, data);
}