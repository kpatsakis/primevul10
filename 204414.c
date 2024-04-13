static void sctp_ulpq_flush(struct sctp_ulpq *ulpq)
{
	struct sk_buff *skb;
	struct sctp_ulpevent *event;

	while ((skb = __skb_dequeue(&ulpq->lobby)) != NULL) {
		event = sctp_skb2event(skb);
		sctp_ulpevent_free(event);
	}

	while ((skb = __skb_dequeue(&ulpq->reasm)) != NULL) {
		event = sctp_skb2event(skb);
		sctp_ulpevent_free(event);
	}

}