gdata_service_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GDataServicePrivate *priv = GDATA_SERVICE (object)->priv;

	switch (property_id) {
		case PROP_PROXY_URI:
			g_value_set_boxed (value, gdata_service_get_proxy_uri (GDATA_SERVICE (object)));
			break;
		case PROP_TIMEOUT:
			g_value_set_uint (value, gdata_service_get_timeout (GDATA_SERVICE (object)));
			break;
		case PROP_LOCALE:
			g_value_set_string (value, priv->locale);
			break;
		case PROP_AUTHORIZER:
			g_value_set_object (value, priv->authorizer);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}