gdata_service_dispose (GObject *object)
{
	GDataServicePrivate *priv = GDATA_SERVICE (object)->priv;

	if (priv->authorizer != NULL)
		g_object_unref (priv->authorizer);
	priv->authorizer = NULL;

	if (priv->session != NULL)
		g_object_unref (priv->session);
	priv->session = NULL;

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_service_parent_class)->dispose (object);
}