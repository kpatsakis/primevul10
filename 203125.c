gdata_service_init (GDataService *self)
{
	self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GDATA_TYPE_SERVICE, GDataServicePrivate);
	self->priv->session = _gdata_service_build_session ();

	/* Debug log handling */
	g_log_set_handler (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, (GLogFunc) debug_handler, self);

	/* Proxy the SoupSession's proxy-uri and timeout properties */
	g_signal_connect (self->priv->session, "notify::proxy-uri", (GCallback) notify_proxy_uri_cb, self);
	g_signal_connect (self->priv->session, "notify::timeout", (GCallback) notify_timeout_cb, self);
}