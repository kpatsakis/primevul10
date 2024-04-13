gdata_service_set_timeout (GDataService *self, guint timeout)
{
	g_return_if_fail (GDATA_IS_SERVICE (self));
	g_object_set (self->priv->session, SOUP_SESSION_TIMEOUT, timeout, NULL);
	g_object_notify (G_OBJECT (self), "timeout");
}