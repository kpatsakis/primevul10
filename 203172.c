_gdata_service_build_message (GDataService *self, GDataAuthorizationDomain *domain, const gchar *method, const gchar *uri,
                              const gchar *etag, gboolean etag_if_match)
{
	SoupMessage *message;
	GDataServiceClass *klass;

	/* Create the message */
	message = soup_message_new (method, uri);

	/* Make sure subclasses set their headers */
	klass = GDATA_SERVICE_GET_CLASS (self);
	if (klass->append_query_headers != NULL)
		klass->append_query_headers (self, domain, message);

	/* Append the ETag header if possible */
	if (etag != NULL)
		soup_message_headers_append (message->request_headers, (etag_if_match == TRUE) ? "If-Match" : "If-None-Match", etag);

	return message;
}