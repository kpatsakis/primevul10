void freeService(service_info *in)
{
	if (in) {
		if (in->serviceType)
			ixmlFreeDOMString(in->serviceType);

		if (in->serviceId)
			ixmlFreeDOMString(in->serviceId);

		if (in->SCPDURL)
			free(in->SCPDURL);

		if (in->controlURL)
			free(in->controlURL);

		if (in->eventURL)
			free(in->eventURL);

		if (in->UDN)
			ixmlFreeDOMString(in->UDN);

		if (in->subscriptionList)
			freeSubscriptionList(in->subscriptionList);

		in->TotalSubscriptions = 0;
		free(in);
	}
}