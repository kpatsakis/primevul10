theme_adium_get_property (GObject    *object,
			  guint       param_id,
			  GValue     *value,
			  GParamSpec *pspec)
{
	EmpathyThemeAdiumPriv *priv = GET_PRIV (object);

	switch (param_id) {
	case PROP_ADIUM_DATA:
		g_value_set_boxed (value, priv->data);
		break;
	case PROP_VARIANT:
		g_value_set_string (value, priv->variant);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	};
}