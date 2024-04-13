real_parse_error_response (GDataService *self, GDataOperationType operation_type, guint status, const gchar *reason_phrase,
                           const gchar *response_body, gint length, GError **error)
{
	/* We prefer to include the @response_body in the error message, but if it's empty, fall back to the @reason_phrase */
	if (response_body == NULL || *response_body == '\0')
		response_body = reason_phrase;

	/* See: http://code.google.com/apis/gdata/docs/2.0/reference.html#HTTPStatusCodes */
	switch (status) {
		case SOUP_STATUS_CANT_RESOLVE:
		case SOUP_STATUS_CANT_CONNECT:
		case SOUP_STATUS_SSL_FAILED:
		case SOUP_STATUS_IO_ERROR:
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_NETWORK_ERROR,
			             _("Cannot connect to the service's server."));
			return;
		case SOUP_STATUS_CANT_RESOLVE_PROXY:
		case SOUP_STATUS_CANT_CONNECT_PROXY:
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROXY_ERROR,
			             _("Cannot connect to the proxy server."));
			return;
		case SOUP_STATUS_MALFORMED:
		case SOUP_STATUS_BAD_REQUEST: /* 400 */
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
			             /* Translators: the parameter is an error message returned by the server. */
			             _("Invalid request URI or header, or unsupported nonstandard parameter: %s"), response_body);
			return;
		case SOUP_STATUS_UNAUTHORIZED: /* 401 */
		case SOUP_STATUS_FORBIDDEN: /* 403 */
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_AUTHENTICATION_REQUIRED,
			             /* Translators: the parameter is an error message returned by the server. */
			             _("Authentication required: %s"), response_body);
			return;
		case SOUP_STATUS_NOT_FOUND: /* 404 */
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_NOT_FOUND,
			             /* Translators: the parameter is an error message returned by the server. */
			             _("The requested resource was not found: %s"), response_body);
			return;
		case SOUP_STATUS_CONFLICT: /* 409 */
		case SOUP_STATUS_PRECONDITION_FAILED: /* 412 */
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_CONFLICT,
			             /* Translators: the parameter is an error message returned by the server. */
			             _("The entry has been modified since it was downloaded: %s"), response_body);
			return;
		case SOUP_STATUS_INTERNAL_SERVER_ERROR: /* 500 */
		default:
			/* We'll fall back to generic errors, below */
			break;
	}

	/* If the error hasn't been handled already, throw a generic error */
	switch (operation_type) {
		case GDATA_OPERATION_AUTHENTICATION:
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
			             /* Translators: the first parameter is an HTTP status,
			              * and the second is an error message returned by the server. */
			             _("Error code %u when authenticating: %s"), status, response_body);
			break;
		case GDATA_OPERATION_QUERY:
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
			             /* Translators: the first parameter is an HTTP status,
			              * and the second is an error message returned by the server. */
			             _("Error code %u when querying: %s"), status, response_body);
			break;
		case GDATA_OPERATION_INSERTION:
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
			             /* Translators: the first parameter is an HTTP status,
			              * and the second is an error message returned by the server. */
			             _("Error code %u when inserting an entry: %s"), status, response_body);
			break;
		case GDATA_OPERATION_UPDATE:
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
			             /* Translators: the first parameter is an HTTP status,
			              * and the second is an error message returned by the server. */
			             _("Error code %u when updating an entry: %s"), status, response_body);
			break;
		case GDATA_OPERATION_DELETION:
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
			             /* Translators: the first parameter is an HTTP status,
			              * and the second is an error message returned by the server. */
			             _("Error code %u when deleting an entry: %s"), status, response_body);
			break;
		case GDATA_OPERATION_DOWNLOAD:
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
			             /* Translators: the first parameter is an HTTP status,
			              * and the second is an error message returned by the server. */
			             _("Error code %u when downloading: %s"), status, response_body);
			break;
		case GDATA_OPERATION_UPLOAD:
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_PROTOCOL_ERROR,
			             /* Translators: the first parameter is an HTTP status,
			              * and the second is an error message returned by the server. */
			             _("Error code %u when uploading: %s"), status, response_body);
			break;
		case GDATA_OPERATION_BATCH:
			g_set_error (error, GDATA_SERVICE_ERROR, GDATA_SERVICE_ERROR_WITH_BATCH_OPERATION,
			             /* Translators: the first parameter is a HTTP status,
			              * and the second is an error message returned by the server. */
			             _("Error code %u when running a batch operation: %s"), status, response_body);
			break;
		default:
			/* We should not be called with anything other than the above operation types */
			g_assert_not_reached ();
	}
}