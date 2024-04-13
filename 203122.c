gdata_service_get_authorizer (GDataService *self)
{
	g_return_val_if_fail (GDATA_IS_SERVICE (self), NULL);

	return self->priv->authorizer;
}