gdata_service_query_single_entry (GDataService *self, GDataAuthorizationDomain *domain, const gchar *entry_id, GDataQuery *query, GType entry_type,
                                  GCancellable *cancellable, GError **error)
{
	GDataEntryClass *klass;
	GDataEntry *entry;
	gchar *entry_uri;
	SoupMessage *message;

	g_return_val_if_fail (GDATA_IS_SERVICE (self), NULL);
	g_return_val_if_fail (domain == NULL || GDATA_IS_AUTHORIZATION_DOMAIN (domain), NULL);
	g_return_val_if_fail (entry_id != NULL, NULL);
	g_return_val_if_fail (query == NULL || GDATA_IS_QUERY (query), NULL);
	g_return_val_if_fail (g_type_is_a (entry_type, GDATA_TYPE_ENTRY) == TRUE, NULL);
	g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), NULL);
	g_return_val_if_fail (error == NULL || *error == NULL, NULL);

	/* Query for just the specified entry */
	klass = GDATA_ENTRY_CLASS (g_type_class_ref (entry_type));
	g_assert (klass->get_entry_uri != NULL);

	entry_uri = klass->get_entry_uri (entry_id);
	message = _gdata_service_query (GDATA_SERVICE (self), domain, entry_uri, query, cancellable, error);
	g_free (entry_uri);

	if (message == NULL) {
		g_type_class_unref (klass);
		return NULL;
	}

	g_assert (message->response_body->data != NULL);
	entry = GDATA_ENTRY (gdata_parsable_new_from_xml (entry_type, message->response_body->data, message->response_body->length, error));
	g_object_unref (message);
	g_type_class_unref (klass);

	return entry;
}