subscription *GetSubscriptionSID(const Upnp_SID sid, service_info *service)
{
	subscription *next = service->subscriptionList;
	subscription *previous = NULL;
	subscription *found = NULL;
	time_t current_time;

	while (next && !found) {
		if (!strcmp(next->sid, sid))
			found = next;
		else {
			previous = next;
			next = next->next;
		}
	}
	if (found) {
		/* get the current_time */
		time(&current_time);
		if (found->expireTime && found->expireTime < current_time) {
			if (previous) {
				previous->next = found->next;
			} else {
				service->subscriptionList = found->next;
			}
			found->next = NULL;
			freeSubscriptionList(found);
			found = NULL;
			service->TotalSubscriptions--;
		}
	}
	return found;
}