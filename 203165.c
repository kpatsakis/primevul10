gdata_service_query_async (GDataService *self, GDataAuthorizationDomain *domain, const gchar *feed_uri, GDataQuery *query, GType entry_type,
                           GCancellable *cancellable, GDataQueryProgressCallback progress_callback, gpointer progress_user_data,
                           GDestroyNotify destroy_progress_user_data, GAsyncReadyCallback callback, gpointer user_data)
{
	GSimpleAsyncResult *result;
	QueryAsyncData *data;

	g_return_if_fail (GDATA_IS_SERVICE (self));
	g_return_if_fail (domain == NULL || GDATA_IS_AUTHORIZATION_DOMAIN (domain));
	g_return_if_fail (feed_uri != NULL);
	g_return_if_fail (g_type_is_a (entry_type, GDATA_TYPE_ENTRY));
	g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));
	g_return_if_fail (callback != NULL);

	data = g_slice_new (QueryAsyncData);
	data->domain = (domain != NULL) ? g_object_ref (domain) : NULL;
	data->feed_uri = g_strdup (feed_uri);
	data->query = (query != NULL) ? g_object_ref (query) : NULL;
	data->entry_type = entry_type;
	data->feed = NULL;
	data->progress_callback = progress_callback;
	data->progress_user_data = progress_user_data;
	data->destroy_progress_user_data = destroy_progress_user_data;

	result = g_simple_async_result_new (G_OBJECT (self), callback, user_data, gdata_service_query_async);
	g_simple_async_result_set_op_res_gpointer (result, data, (GDestroyNotify) query_async_data_free);
	g_simple_async_result_run_in_thread (result, (GSimpleAsyncThreadFunc) query_thread, G_PRIORITY_DEFAULT, cancellable);
	g_object_unref (result);
}