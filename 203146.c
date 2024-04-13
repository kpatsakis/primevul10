query_single_entry_thread (GSimpleAsyncResult *result, GDataService *service, GCancellable *cancellable)
{
	GDataEntry *entry;
	GError *error = NULL;
	QuerySingleEntryAsyncData *data = g_simple_async_result_get_op_res_gpointer (result);

	/* Execute the query and return */
	entry = gdata_service_query_single_entry (service, data->domain, data->entry_id, data->query, data->entry_type, cancellable, &error);
	if (entry == NULL && error != NULL) {
		g_simple_async_result_set_from_error (result, error);
		g_error_free (error);
		return;
	}

	g_simple_async_result_set_op_res_gpointer (result, entry, (GDestroyNotify) g_object_unref);
}