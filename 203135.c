gdata_service_insert_entry_async (GDataService *self, GDataAuthorizationDomain *domain, const gchar *upload_uri, GDataEntry *entry,
                                  GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data)
{
	GSimpleAsyncResult *result;
	InsertEntryAsyncData *data;

	g_return_if_fail (GDATA_IS_SERVICE (self));
	g_return_if_fail (domain == NULL || GDATA_IS_AUTHORIZATION_DOMAIN (domain));
	g_return_if_fail (upload_uri != NULL);
	g_return_if_fail (GDATA_IS_ENTRY (entry));
	g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

	data = g_slice_new (InsertEntryAsyncData);
	data->domain = (domain != NULL) ? g_object_ref (domain) : NULL;
	data->upload_uri = g_strdup (upload_uri);
	data->entry = g_object_ref (entry);

	result = g_simple_async_result_new (G_OBJECT (self), callback, user_data, gdata_service_insert_entry_async);
	g_simple_async_result_set_op_res_gpointer (result, data, (GDestroyNotify) insert_entry_async_data_free);
	g_simple_async_result_run_in_thread (result, (GSimpleAsyncThreadFunc) insert_entry_thread, G_PRIORITY_DEFAULT, cancellable);
	g_object_unref (result);
}