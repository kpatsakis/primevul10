_gdata_service_send_message (GDataService *self, SoupMessage *message, GCancellable *cancellable, GError **error)
{
	/* Based on code from evolution-data-server's libgdata:
	 *  Ebby Wiselyn <ebbywiselyn@gmail.com>
	 *  Jason Willis <zenbrother@gmail.com>
	 *
	 * Copyright (C) 1999-2008 Novell, Inc. (www.novell.com)
	 */

	soup_message_set_flags (message, SOUP_MESSAGE_NO_REDIRECT);
	_gdata_service_actually_send_message (self->priv->session, message, cancellable, error);
	soup_message_set_flags (message, 0);

	/* Handle redirections specially so we don't lose our custom headers when making the second request */
	if (SOUP_STATUS_IS_REDIRECTION (message->status_code)) {
		SoupURI *new_uri;
		const gchar *new_location;

		new_location = soup_message_headers_get_one (message->response_headers, "Location");
		g_return_val_if_fail (new_location != NULL, SOUP_STATUS_NONE);

		new_uri = soup_uri_new_with_base (soup_message_get_uri (message), new_location);
		if (new_uri == NULL) {
			gchar *uri_string = soup_uri_to_string (new_uri, FALSE);
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
			             /* Translators: the parameter is the URI which is invalid. */
			             _("Invalid redirect URI: %s"), uri_string);
			g_free (uri_string);
			return SOUP_STATUS_NONE;
		}

		soup_message_set_uri (message, new_uri);
		soup_uri_free (new_uri);

		/* Send the message again */
		_gdata_service_actually_send_message (self->priv->session, message, cancellable, error);
	}

	/* Not authorised, or authorisation has expired. If we were authorised in the first place, attempt to refresh the authorisation and
	 * try sending the message again (but only once, so we don't get caught in an infinite loop of denied authorisation errors).
	 *
	 * Note that we have to re-process the message with the authoriser so that its authorisation headers get updated after the refresh
	 * (bgo#653535). */
	if (message->status_code == SOUP_STATUS_UNAUTHORIZED) {
		GDataAuthorizer *authorizer = self->priv->authorizer;

		if (authorizer != NULL && gdata_authorizer_refresh_authorization (authorizer, cancellable, NULL) == TRUE) {
			GDataAuthorizationDomain *domain;

			/* Re-process the request */
			domain = g_object_get_data (G_OBJECT (message), "gdata-authorization-domain");
			g_assert (domain == NULL || GDATA_IS_AUTHORIZATION_DOMAIN (domain));

			gdata_authorizer_process_request (authorizer, domain, message);

			/* Send the message again */
			g_clear_error (error);
			_gdata_service_actually_send_message (self->priv->session, message, cancellable, error);
		}
	}

	return message->status_code;
}