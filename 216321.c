subscription *GetNextSubscription(service_info *service, subscription *current)
{
	time_t current_time;
	subscription *next = NULL;
	subscription *previous = NULL;
	int notDone = 1;

	/* get the current_time */
	time(&current_time);
	while (notDone && current) {
		previous = current;
		current = current->next;

		if (!current) {
			notDone = 0;
			next = current;
		} else if (current->expireTime &&
			   current->expireTime < current_time) {
			previous->next = current->next;
			current->next = NULL;
			freeSubscriptionList(current);
			current = previous;
			service->TotalSubscriptions--;
		} else if (current->active) {
			notDone = 0;
			next = current;
		}
	}
	return next;
}