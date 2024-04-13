_gdata_service_query (GDataService *self, GDataAuthorizationDomain *domain, const gchar *feed_uri, GDataQuery *query,
                      GCancellable *cancellable, GError **error)
{
	SoupMessage *message;
	guint status;
	const gchar *etag = NULL;

	/* Append the ETag header if possible */
	if (query != NULL)
		etag = gdata_query_get_etag (query);

	/* Build the message */
	if (query != NULL) {
		gchar *query_uri = gdata_query_get_query_uri (query, feed_uri);
		message = _gdata_service_build_message (self, domain, SOUP_METHOD_GET, query_uri, etag, FALSE);
		g_free (query_uri);
	} else {
		message = _gdata_service_build_message (self, domain, SOUP_METHOD_GET, feed_uri, etag, FALSE);
	}

	/* Note that cancellation only applies to network activity; not to the processing done afterwards */
	status = _gdata_service_send_message (self, message, cancellable, error);

	if (status == SOUP_STATUS_NOT_MODIFIED || status == SOUP_STATUS_CANCELLED) {
		/* Not modified (ETag has worked), or cancelled (in which case the error has been set) */
		g_object_unref (message);
		return NULL;
	} else if (status != SOUP_STATUS_OK) {
		/* Error */
		GDataServiceClass *klass = GDATA_SERVICE_GET_CLASS (self);
		g_assert (klass->parse_error_response != NULL);
		klass->parse_error_response (self, GDATA_OPERATION_QUERY, status, message->reason_phrase, message->response_body->data,
		                             message->response_body->length, error);
		g_object_unref (message);
		return NULL;
	}

	return message;
}