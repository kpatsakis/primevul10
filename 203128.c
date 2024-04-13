notify_proxy_uri_cb (GObject *gobject, GParamSpec *pspec, GObject *self)
{
	g_object_notify (self, "proxy-uri");
}