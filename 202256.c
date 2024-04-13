int rtnetlink_put_metrics(struct sk_buff *skb, u32 *metrics)
{
	struct rtattr *mx = (struct rtattr*)skb->tail;
	int i;

	RTA_PUT(skb, RTA_METRICS, 0, NULL);
	for (i=0; i<RTAX_MAX; i++) {
		if (metrics[i])
			RTA_PUT(skb, i+1, sizeof(u32), metrics+i);
	}
	mx->rta_len = skb->tail - (u8*)mx;
	if (mx->rta_len == RTA_LENGTH(0))
		skb_trim(skb, (u8*)mx - skb->data);
	return 0;

rtattr_failure:
	skb_trim(skb, (u8*)mx - skb->data);
	return -1;
}