empathy_theme_adium_new (EmpathyAdiumData *data,
			 const gchar *variant)
{
	g_return_val_if_fail (data != NULL, NULL);

	return g_object_new (EMPATHY_TYPE_THEME_ADIUM,
			     "adium-data", data,
			     "variant", variant,
			     NULL);
}