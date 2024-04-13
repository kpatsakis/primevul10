gdata_service_set_proxy_uri (GDataService *self, SoupURI *proxy_uri)
{
	g_return_if_fail (GDATA_IS_SERVICE (self));
	g_object_set (self->priv->session, SOUP_SESSION_PROXY_URI, proxy_uri, NULL);
	g_object_notify (G_OBJECT (self), "proxy-uri");
}