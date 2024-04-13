gdata_service_get_authorization_domains (GType service_type)
{
	GDataServiceClass *klass;
	GList *domains = NULL;

	g_return_val_if_fail (g_type_is_a (service_type, GDATA_TYPE_SERVICE), NULL);

	klass = GDATA_SERVICE_CLASS (g_type_class_ref (service_type));
	if (klass->get_authorization_domains != NULL) {
		domains = klass->get_authorization_domains ();
	}
	g_type_class_unref (klass);

	return domains;
}