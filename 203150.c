gdata_service_delete_entry_async (GDataService *self, GDataAuthorizationDomain *domain, GDataEntry *entry,
                                  GCancellable *cancellable, GAsyncReadyCallback callback, gpointer user_data)
{
	GSimpleAsyncResult *result;
	DeleteEntryAsyncData *data;

	g_return_if_fail (GDATA_IS_SERVICE (self));
	g_return_if_fail (domain == NULL || GDATA_IS_AUTHORIZATION_DOMAIN (domain));
	g_return_if_fail (GDATA_IS_ENTRY (entry));
	g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

	data = g_slice_new (DeleteEntryAsyncData);
	data->domain = (domain != NULL) ? g_object_ref (domain) : NULL;
	data->entry = g_object_ref (entry);

	result = g_simple_async_result_new (G_OBJECT (self), callback, user_data, gdata_service_delete_entry_async);
	g_simple_async_result_set_op_res_gpointer (result, data, (GDestroyNotify) delete_entry_async_data_free);
	g_simple_async_result_run_in_thread (result, (GSimpleAsyncThreadFunc) delete_entry_thread, G_PRIORITY_DEFAULT, cancellable);
	g_object_unref (result);
}