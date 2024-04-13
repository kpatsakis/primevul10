gdata_service_query (GDataService *self, GDataAuthorizationDomain *domain, const gchar *feed_uri, GDataQuery *query, GType entry_type,
                     GCancellable *cancellable, GDataQueryProgressCallback progress_callback, gpointer progress_user_data, GError **error)
{
	g_return_val_if_fail (GDATA_IS_SERVICE (self), NULL);
	g_return_val_if_fail (domain == NULL || GDATA_IS_AUTHORIZATION_DOMAIN (domain), NULL);
	g_return_val_if_fail (feed_uri != NULL, NULL);
	g_return_val_if_fail (g_type_is_a (entry_type, GDATA_TYPE_ENTRY), NULL);
	g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	return __gdata_service_query (self, domain, feed_uri, query, entry_type, cancellable, progress_callback, progress_user_data, error, FALSE);
}