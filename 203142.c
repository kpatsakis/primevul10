notify_timeout_cb (GObject *gobject, GParamSpec *pspec, GObject *self)
{
	g_object_notify (self, "timeout");
}