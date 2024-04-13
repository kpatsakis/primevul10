gdata_service_get_timeout (GDataService *self)
{
	guint timeout;

	g_return_val_if_fail (GDATA_IS_SERVICE (self), 0);

	g_object_get (self->priv->session, SOUP_SESSION_TIMEOUT, &timeout, NULL);

	return timeout;
}