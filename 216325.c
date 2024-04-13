void RemoveSubscriptionSID(Upnp_SID sid, service_info *service)
{
	subscription *finger = service->subscriptionList;
	subscription *previous = NULL;

	while (finger) {
		if (!strcmp(sid, finger->sid)) {
			if (previous) {
				previous->next = finger->next;
			} else {
				service->subscriptionList = finger->next;
			}
			finger->next = NULL;
			freeSubscriptionList(finger);
			finger = NULL;
			service->TotalSubscriptions--;
		} else {
			previous = finger;
			finger = finger->next;
		}
	}
}