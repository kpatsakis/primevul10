query_thread (GSimpleAsyncResult *result, GDataService *service, GCancellable *cancellable)
{
	GError *error = NULL;
	QueryAsyncData *data = g_simple_async_result_get_op_res_gpointer (result);

	/* Execute the query and return */
	data->feed = __gdata_service_query (service, data->domain, data->feed_uri, data->query, data->entry_type, cancellable,
	                                    data->progress_callback, data->progress_user_data, &error, TRUE);
	if (data->feed == NULL && error != NULL) {
		g_simple_async_result_set_from_error (result, error);
		g_error_free (error);
	}

	if (data->destroy_progress_user_data != NULL) {
		data->destroy_progress_user_data (data->progress_user_data);
	}
}