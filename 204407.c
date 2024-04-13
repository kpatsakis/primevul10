static __u16 sctp_ulpq_renege_frags(struct sctp_ulpq *ulpq, __u16 needed)
{
	__u16 freed = 0;
	__u32 tsn;
	struct sk_buff *skb;
	struct sctp_ulpevent *event;
	struct sctp_tsnmap *tsnmap;

	tsnmap = &ulpq->asoc->peer.tsn_map;

	/* Walk backwards through the list, reneges the newest tsns. */
	while ((skb = __skb_dequeue_tail(&ulpq->reasm)) != NULL) {
		freed += skb_headlen(skb);
		event = sctp_skb2event(skb);
		tsn = event->tsn;

		sctp_ulpevent_free(event);
		sctp_tsnmap_renege(tsnmap, tsn);
		if (freed >= needed)
			return freed;
	}

	return freed;
}