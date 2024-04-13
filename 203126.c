gdata_service_delete_entry (GDataService *self, GDataAuthorizationDomain *domain, GDataEntry *entry, GCancellable *cancellable, GError **error)
{
	GDataLink *_link;
	SoupMessage *message;
	guint status;
	gchar *fixed_uri;

	g_return_val_if_fail (GDATA_IS_SERVICE (self), FALSE);
	g_return_val_if_fail (domain == NULL || GDATA_IS_AUTHORIZATION_DOMAIN (domain), FALSE);
	g_return_val_if_fail (GDATA_IS_ENTRY (entry), FALSE);
	g_return_val_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable), FALSE);
	g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

	/* Get the edit URI. We have to fix it to always use HTTPS as YouTube videos appear to incorrectly return a HTTP URI as their edit URI. */
	_link = gdata_entry_look_up_link (entry, GDATA_LINK_EDIT);
	g_assert (_link != NULL);

	fixed_uri = _gdata_service_fix_uri_scheme (gdata_link_get_uri (_link));
	message = _gdata_service_build_message (self, domain, SOUP_METHOD_DELETE, fixed_uri, gdata_entry_get_etag (entry), TRUE);
	g_free (fixed_uri);

	/* Send the message */
	status = _gdata_service_send_message (self, message, cancellable, error);

	if (status == SOUP_STATUS_NONE || status == SOUP_STATUS_CANCELLED) {
		/* Redirect error or cancelled */
		g_object_unref (message);
		return FALSE;
	} else if (status != SOUP_STATUS_OK) {
		/* Error */
		GDataServiceClass *klass = GDATA_SERVICE_GET_CLASS (self);
		g_assert (klass->parse_error_response != NULL);
		klass->parse_error_response (self, GDATA_OPERATION_DELETION, status, message->reason_phrase, message->response_body->data,
		                             message->response_body->length, error);
		g_object_unref (message);
		return FALSE;
	}

	g_object_unref (message);

	return TRUE;
}