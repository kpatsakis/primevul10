gdata_service_set_authorizer (GDataService *self, GDataAuthorizer *authorizer)
{
	GDataServicePrivate *priv = self->priv;

	g_return_if_fail (GDATA_IS_SERVICE (self));
	g_return_if_fail (authorizer == NULL || GDATA_IS_AUTHORIZER (authorizer));

	if (priv->authorizer != NULL) {
		g_object_unref (priv->authorizer);
	}

	priv->authorizer = authorizer;

	if (priv->authorizer != NULL) {
		g_object_ref (priv->authorizer);
	}

	g_object_notify (G_OBJECT (self), "authorizer");
}