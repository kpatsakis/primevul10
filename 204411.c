static inline void sctp_ulpq_store_ordered(struct sctp_ulpq *ulpq,
					   struct sctp_ulpevent *event)
{
	struct sk_buff *pos;
	struct sctp_ulpevent *cevent;
	__u16 sid, csid;
	__u16 ssn, cssn;

	pos = skb_peek_tail(&ulpq->lobby);
	if (!pos) {
		__skb_queue_tail(&ulpq->lobby, sctp_event2skb(event));
		return;
	}

	sid = event->stream;
	ssn = event->ssn;
	
	cevent = (struct sctp_ulpevent *) pos->cb;
	csid = cevent->stream;
	cssn = cevent->ssn;
	if (sid > csid) {
		__skb_queue_tail(&ulpq->lobby, sctp_event2skb(event));
		return;
	}

	if ((sid == csid) && SSN_lt(cssn, ssn)) {
		__skb_queue_tail(&ulpq->lobby, sctp_event2skb(event));
		return;
	}

	/* Find the right place in this list.  We store them by
	 * stream ID and then by SSN.
	 */
	skb_queue_walk(&ulpq->lobby, pos) {
		cevent = (struct sctp_ulpevent *) pos->cb;
		csid = cevent->stream;
		cssn = cevent->ssn;

		if (csid > sid)
			break;
		if (csid == sid && SSN_lt(ssn, cssn))
			break;
	}


	/* Insert before pos. */
	__skb_insert(sctp_event2skb(event), pos->prev, pos, &ulpq->lobby);

}