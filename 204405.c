static inline void sctp_ulpq_retrieve_ordered(struct sctp_ulpq *ulpq,
					      struct sctp_ulpevent *event)
{
	struct sk_buff_head *event_list;
	struct sk_buff *pos, *tmp;
	struct sctp_ulpevent *cevent;
	struct sctp_stream *in;
	__u16 sid, csid;
	__u16 ssn, cssn;

	sid = event->stream;
	ssn = event->ssn;
	in  = &ulpq->asoc->ssnmap->in;

	event_list = (struct sk_buff_head *) sctp_event2skb(event)->prev;

	/* We are holding the chunks by stream, by SSN.  */
	sctp_skb_for_each(pos, &ulpq->lobby, tmp) {
		cevent = (struct sctp_ulpevent *) pos->cb;
		csid = cevent->stream;
		cssn = cevent->ssn;

		/* Have we gone too far?  */
		if (csid > sid)
			break;

		/* Have we not gone far enough?  */
		if (csid < sid)
			continue;

		if (cssn != sctp_ssn_peek(in, sid))
			break;

		/* Found it, so mark in the ssnmap. */
		sctp_ssn_next(in, sid);

		__skb_unlink(pos, &ulpq->lobby);

		/* Attach all gathered skbs to the event.  */
		__skb_queue_tail(event_list, pos);
	}
}