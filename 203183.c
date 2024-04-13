gdata_service_is_authorized (GDataService *self)
{
	GList *domains, *i;
	gboolean authorised = TRUE;

	g_return_val_if_fail (GDATA_IS_SERVICE (self), FALSE);

	/* If we don't have an authoriser set, we can't be authorised */
	if (self->priv->authorizer == NULL) {
		return FALSE;
	}

	domains = gdata_service_get_authorization_domains (G_OBJECT_TYPE (self));

	/* Find any domains which we're not authorised for */
	for (i = domains; i != NULL; i = i->next) {
		if (gdata_authorizer_is_authorized_for_domain (self->priv->authorizer, GDATA_AUTHORIZATION_DOMAIN (i->data)) == FALSE) {
			authorised = FALSE;
			break;
		}
	}

	g_list_free (domains);

	return authorised;
}