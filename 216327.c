subscription *GetFirstSubscription(service_info *service)
{
	subscription temp;
	subscription *next = NULL;

	temp.next = service->subscriptionList;
	next = GetNextSubscription(service, &temp);
	service->subscriptionList = temp.next;
	/* service->subscriptionList = next; */

	return next;
}