gdata_service_finalize (GObject *object)
{
	GDataServicePrivate *priv = GDATA_SERVICE (object)->priv;

	g_free (priv->locale);

	/* Chain up to the parent class */
	G_OBJECT_CLASS (gdata_service_parent_class)->finalize (object);
}