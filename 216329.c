void freeSubscriptionList(subscription *head)
{
	subscription *next = NULL;

	while (head) {
		next = head->next;
		freeSubscription(head);
		free(head);
		head = next;
	}
}