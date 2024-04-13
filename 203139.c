real_append_query_headers (GDataService *self, GDataAuthorizationDomain *domain, SoupMessage *message)
{
	g_assert (message != NULL);

	/* Set the authorisation header */
	if (self->priv->authorizer != NULL) {
		gdata_authorizer_process_request (self->priv->authorizer, domain, message);

		if (domain != NULL) {
			/* Store the authorisation domain on the message so that we can access it again after refreshing authorisation if necessary.
			 * See _gdata_service_send_message(). */
			g_object_set_data_full (G_OBJECT (message), "gdata-authorization-domain", g_object_ref (domain),
			                        (GDestroyNotify) g_object_unref);
		}
	}

	/* Set the GData-Version header to tell it we want to use the v2 API */
	soup_message_headers_append (message->request_headers, "GData-Version", GDATA_SERVICE_GET_CLASS (self)->api_version);

	/* Set the locale, if it's been set for the service */
	if (self->priv->locale != NULL)
		soup_message_headers_append (message->request_headers, "Accept-Language", self->priv->locale);
}