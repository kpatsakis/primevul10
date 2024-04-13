void freeSubscription(subscription *sub)
{
	if (sub) {
		free_URL_list(&sub->DeliveryURLs);
		freeSubscriptionQueuedEvents(sub);
	}
}