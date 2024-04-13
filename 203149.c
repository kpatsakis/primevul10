gdata_service_set_property (GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	switch (property_id) {
		case PROP_PROXY_URI:
			gdata_service_set_proxy_uri (GDATA_SERVICE (object), g_value_get_boxed (value));
			break;
		case PROP_TIMEOUT:
			gdata_service_set_timeout (GDATA_SERVICE (object), g_value_get_uint (value));
			break;
		case PROP_LOCALE:
			gdata_service_set_locale (GDATA_SERVICE (object), g_value_get_string (value));
			break;
		case PROP_AUTHORIZER:
			gdata_service_set_authorizer (GDATA_SERVICE (object), g_value_get_object (value));
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}