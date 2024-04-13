gdata_service_insert_entry (GDataService *self, GDataAuthorizationDomain *domain, const gchar *upload_uri, GDataEntry *entry,
                            GCancellable *cancellable, GError **error)
{
	GDataEntry *updated_entry;
	SoupMessage *message;
	gchar *upload_data;
	guint status;

	g_return_val_if_fail (GDATA_IS_SERVICE (self), NULL);
	g_return_val_if_fail (domain == NULL || GDATA_IS_AUTHORIZATION_DOMAIN (domain), NULL);
	g_return_val_if_fail (upload_uri != NULL, NULL);
	g_return_val_if_fail (GDATA_IS_ENTRY (entry), NULL);
	g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	if (gdata_entry_is_inserted (entry) == TRUE) {
		g_set_error_literal (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_ENTRY_ALREADY_INSERTED,
		                     _("The entry has already been inserted."));
		return NULL;
	}

	message = _gdata_service_build_message (self, domain, SOUP_METHOD_POST, upload_uri, NULL, FALSE);

	/* Append the data */
	upload_data = gdata_parsable_get_xml (GDATA_PARSABLE (entry));
	soup_message_set_request (message, "application/atom+xml", SOUP_MEMORY_TAKE, upload_data, strlen (upload_data));

	/* Send the message */
	status = _gdata_service_send_message (self, message, cancellable, error);

	if (status == SOUP_STATUS_NONE || status == SOUP_STATUS_CANCELLED) {
		/* Redirect error or cancelled */
		g_object_unref (message);
		return NULL;
	} else if (status != SOUP_STATUS_CREATED) {
		/* Error */
		GDataServiceClass *klass = GDATA_SERVICE_GET_CLASS (self);
		g_assert (klass->parse_error_response != NULL);
		klass->parse_error_response (self, GDATA_OPERATION_INSERTION, status, message->reason_phrase, message->response_body->data,
		                             message->response_body->length, error);
		g_object_unref (message);
		return NULL;
	}

	/* Parse the XML; create and return a new GDataEntry of the same type as @entry */
	g_assert (message->response_body->data != NULL);
	updated_entry = GDATA_ENTRY (gdata_parsable_new_from_xml (G_OBJECT_TYPE (entry), message->response_body->data, message->response_body->length,
	                                                          error));
	g_object_unref (message);

	return updated_entry;
}