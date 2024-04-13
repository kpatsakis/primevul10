gdata_service_update_entry (GDataService *self, GDataAuthorizationDomain *domain, GDataEntry *entry, GCancellable *cancellable, GError **error)
{
	GDataEntry *updated_entry;
	GDataLink *_link;
	SoupMessage *message;
	gchar *upload_data;
	guint status;

	g_return_val_if_fail (GDATA_IS_SERVICE (self), NULL);
	g_return_val_if_fail (domain == NULL || GDATA_IS_AUTHORIZATION_DOMAIN (domain), NULL);
	g_return_val_if_fail (GDATA_IS_ENTRY (entry), NULL);
	g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	/* Get the edit URI */
	_link = gdata_entry_look_up_link (entry, GDATA_LINK_EDIT);
	g_assert (_link != NULL);
	message = _gdata_service_build_message (self, domain, SOUP_METHOD_PUT, gdata_link_get_uri (_link), gdata_entry_get_etag (entry), TRUE);

	/* Append the data */
	upload_data = gdata_parsable_get_xml (GDATA_PARSABLE (entry));
	soup_message_set_request (message, "application/atom+xml", SOUP_MEMORY_TAKE, upload_data, strlen (upload_data));

	/* Send the message */
	status = _gdata_service_send_message (self, message, cancellable, error);

	if (status == SOUP_STATUS_NONE || status == SOUP_STATUS_CANCELLED) {
		/* Redirect error or cancelled */
		g_object_unref (message);
		return NULL;
	} else if (status != SOUP_STATUS_OK) {
		/* Error */
		GDataServiceClass *klass = GDATA_SERVICE_GET_CLASS (self);
		g_assert (klass->parse_error_response != NULL);
		klass->parse_error_response (self, GDATA_OPERATION_UPDATE, status, message->reason_phrase, message->response_body->data,
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