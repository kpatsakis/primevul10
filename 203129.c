gdata_service_query_single_entry_async (GDataService *self, GDataAuthorizationDomain *domain, const gchar *entry_id, GDataQuery *query,
                                        GType entry_type, GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data)
{
	GSimpleAsyncResult *result;
	QuerySingleEntryAsyncData *data;

	g_return_if_fail (GDATA_IS_SERVICE (self));
	g_return_if_fail (domain == NULL || GDATA_IS_AUTHORIZATION_DOMAIN (domain));
	g_return_if_fail (entry_id != NULL);
	g_return_if_fail (query == NULL || GDATA_IS_QUERY (query));
	g_return_if_fail (g_type_is_a (entry_type, GDATA_TYPE_ENTRY) == TRUE);
	g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));
	g_return_if_fail (callback != NULL);

	data = g_slice_new (QuerySingleEntryAsyncData);
	data->domain = (domain != NULL) ? g_object_ref (domain) : NULL;
	data->query = (query != NULL) ? g_object_ref (query) : NULL;
	data->entry_id = g_strdup (entry_id);
	data->entry_type = entry_type;

	result = g_simple_async_result_new (G_OBJECT (self), callback, user_data, gdata_service_query_single_entry_async);
	g_simple_async_result_set_op_res_gpointer (result, data, (GDestroyNotify) query_single_entry_async_data_free);
	g_simple_async_result_run_in_thread (result, (GSimpleAsyncThreadFunc) query_single_entry_thread, G_PRIORITY_DEFAULT, cancellable);
	g_object_unref (result);
}