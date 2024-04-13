int copy_subscription(subscription *in, subscription *out)
{
	int return_code = HTTP_SUCCESS;

	memcpy(out->sid, in->sid, SID_SIZE);
	out->sid[SID_SIZE] = 0;
	out->ToSendEventKey = in->ToSendEventKey;
	out->expireTime = in->expireTime;
	out->active = in->active;
	return_code = copy_URL_list(&in->DeliveryURLs, &out->DeliveryURLs);
	if (return_code != HTTP_SUCCESS) {
		return return_code;
	}
	ListInit(&out->outgoing, 0, 0);
	out->next = NULL;
	return HTTP_SUCCESS;
}