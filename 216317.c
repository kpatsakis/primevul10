void freeServiceList(service_info *head)
{
	service_info *next = NULL;

	while (head) {
		if (head->serviceType)
			ixmlFreeDOMString(head->serviceType);
		if (head->serviceId)
			ixmlFreeDOMString(head->serviceId);
		if (head->SCPDURL)
			free(head->SCPDURL);
		if (head->controlURL)
			free(head->controlURL);
		if (head->eventURL)
			free(head->eventURL);
		if (head->UDN)
			ixmlFreeDOMString(head->UDN);
		if (head->subscriptionList)
			freeSubscriptionList(head->subscriptionList);

		head->TotalSubscriptions = 0;
		next = head->next;
		free(head);
		head = next;
	}
}