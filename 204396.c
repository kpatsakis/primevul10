static inline struct sctp_ulpevent *sctp_ulpq_retrieve_partial(struct sctp_ulpq *ulpq)
{
	struct sk_buff *pos, *last_frag, *first_frag;
	struct sctp_ulpevent *cevent;
	__u32 ctsn, next_tsn;
	int is_last;
	struct sctp_ulpevent *retval;

	/* The chunks are held in the reasm queue sorted by TSN.
	 * Walk through the queue sequentially and look for the first
	 * sequence of fragmented chunks.
	 */

	if (skb_queue_empty(&ulpq->reasm))
		return NULL;

	last_frag = first_frag = NULL;
	retval = NULL;
	next_tsn = 0;
	is_last = 0;

	skb_queue_walk(&ulpq->reasm, pos) {
		cevent = sctp_skb2event(pos);
		ctsn = cevent->tsn;

		switch (cevent->msg_flags & SCTP_DATA_FRAG_MASK) {
		case SCTP_DATA_MIDDLE_FRAG:
			if (!first_frag) {
				first_frag = pos;
				next_tsn = ctsn + 1;
				last_frag = pos;
			} else if (next_tsn == ctsn)
				next_tsn++;
			else
				goto done;
			break;
		case SCTP_DATA_LAST_FRAG:
			if (!first_frag)
				first_frag = pos;
			else if (ctsn != next_tsn)
				goto done;
			last_frag = pos;
			is_last = 1;
			goto done;
		default:
			return NULL;
		};
	}

	/* We have the reassembled event. There is no need to look
	 * further.
	 */
done:
	retval = sctp_make_reassembled_event(&ulpq->reasm, first_frag, last_frag);
	if (retval && is_last)
		retval->msg_flags |= MSG_EOR;

	return retval;
}