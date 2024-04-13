service_info *FindServiceId(
	service_table *table, const char *serviceId, const char *UDN)
{
	service_info *finger = NULL;

	if (table) {
		finger = table->serviceList;
		while (finger) {
			if (!strcmp(serviceId, finger->serviceId) &&
				!strcmp(UDN, finger->UDN)) {
				return finger;
			}
			finger = finger->next;
		}
	}

	return NULL;
}