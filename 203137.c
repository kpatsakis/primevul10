gdata_service_delete_entry_finish (GDataService *self, GAsyncResult *async_result, GError **error)
{
	GSimpleAsyncResult *result = G_SIMPLE_ASYNC_RESULT (async_result);

	g_return_val_if_fail (GDATA_IS_SERVICE (self), FALSE);
	g_return_val_if_fail (G_IS_ASYNC_RESULT (async_result), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	g_warn_if_fail (g_simple_async_result_get_source_tag (result) == gdata_service_delete_entry_async);

	if (g_simple_async_result_propagate_error (result, error) == TRUE)
		return FALSE;

	return g_simple_async_result_get_op_res_gboolean (result);
}