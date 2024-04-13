gdata_service_set_locale (GDataService *self, const gchar *locale)
{
	g_return_if_fail (GDATA_IS_SERVICE (self));

	g_free (self->priv->locale);
	self->priv->locale = g_strdup (locale);
	g_object_notify (G_OBJECT (self), "locale");
}