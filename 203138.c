gdata_service_class_init (GDataServiceClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GDataServicePrivate));

	gobject_class->set_property = gdata_service_set_property;
	gobject_class->get_property = gdata_service_get_property;
	gobject_class->dispose = gdata_service_dispose;
	gobject_class->finalize = gdata_service_finalize;

	klass->api_version = "2";
	klass->feed_type = GDATA_TYPE_FEED;
	klass->append_query_headers = real_append_query_headers;
	klass->parse_error_response = real_parse_error_response;
	klass->get_authorization_domains = NULL; /* equivalent to returning an empty list of domains */

	/**
	 * GDataService:proxy-uri:
	 *
	 * The proxy URI used internally for all network requests.
	 *
	 * Note that if a #GDataAuthorizer is being used with this #GDataService, the authorizer might also need its proxy URI setting.
	 *
	 * Since: 0.2.0
	 **/
	g_object_class_install_property (gobject_class, PROP_PROXY_URI,
	                                 g_param_spec_boxed ("proxy-uri",
	                                                     "Proxy URI", "The proxy URI used internally for all network requests.",
	                                                     SOUP_TYPE_URI,
	                                                     G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataService:timeout:
	 *
	 * A timeout, in seconds, for network operations. If the timeout is exceeded, the operation will be cancelled and
	 * %GDATA_SERVICE_ERROR_NETWORK_ERROR will be returned.
	 *
	 * If the timeout is <code class="literal">0</code>, operations will never time out.
	 *
	 * Note that if a #GDataAuthorizer is being used with this #GDataService, the authorizer might also need its timeout setting.
	 *
	 * Since: 0.7.0
	 **/
	g_object_class_install_property (gobject_class, PROP_TIMEOUT,
	                                 g_param_spec_uint ("timeout",
	                                                    "Timeout", "A timeout, in seconds, for network operations.",
	                                                    0, G_MAXUINT, 0,
	                                                    G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataService:locale:
	 *
	 * The locale to use for network requests, in Unix locale format. (e.g. "en_GB", "cs", "de_DE".) Use %NULL for the default "C" locale
	 * (typically "en_US").
	 *
	 * Typically, this locale will be used by the server-side software to localise results, such as by translating category names, or by choosing
	 * geographically relevant search results. This will vary from service to service.
	 *
	 * The server-side behaviour is undefined if it doesn't support a given locale.
	 *
	 * Since: 0.7.0
	 **/
	g_object_class_install_property (gobject_class, PROP_LOCALE,
	                                 g_param_spec_string ("locale",
	                                                      "Locale", "The locale to use for network requests, in Unix locale format.",
	                                                      NULL,
	                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	/**
	 * GDataService:authorizer:
	 *
	 * An object which implements #GDataAuthorizer. This should have previously been authenticated authorized against this service type (and
	 * potentially other service types). The service will use the authorizer to add an authorization token to each request it performs.
	 *
	 * Your application should call methods on the #GDataAuthorizer object itself in order to authenticate with the Google accounts service and
	 * authorize against this service type. See the documentation for the particular #GDataAuthorizer implementation being used for more details.
	 *
	 * The authorizer for a service can be changed at runtime for a different #GDataAuthorizer object or %NULL without affecting ongoing requests
	 * and operations.
	 *
	 * Note that it's only necessary to set an authorizer on the service if your application is going to make requests of the service which
	 * require authorization. For example, listing the current most popular videos on YouTube does not require authorization, but uploading a
	 * video to YouTube does. It's an unnecessary overhead to require the user to authorize against a service when not strictly required.
	 *
	 * Since: 0.9.0
	 **/
	g_object_class_install_property (gobject_class, PROP_AUTHORIZER,
	                                 g_param_spec_object ("authorizer",
	                                                      "Authorizer", "An authorizer object to provide an authorization token for each request.",
	                                                      GDATA_TYPE_AUTHORIZER,
	                                                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}