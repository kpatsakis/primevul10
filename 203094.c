theme_adium_set_property (GObject      *object,
			  guint         param_id,
			  const GValue *value,
			  GParamSpec   *pspec)
{
	EmpathyThemeAdium *theme = EMPATHY_THEME_ADIUM (object);
	EmpathyThemeAdiumPriv *priv = GET_PRIV (object);

	switch (param_id) {
	case PROP_ADIUM_DATA:
		g_assert (priv->data == NULL);
		priv->data = g_value_dup_boxed (value);
		break;
	case PROP_VARIANT:
		empathy_theme_adium_set_variant (theme, g_value_get_string (value));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
		break;
	};
}