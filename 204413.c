static struct sctp_ulpevent *sctp_ulpq_reasm(struct sctp_ulpq *ulpq,
						struct sctp_ulpevent *event)
{
	struct sctp_ulpevent *retval = NULL;

	/* Check if this is part of a fragmented message.  */
	if (SCTP_DATA_NOT_FRAG == (event->msg_flags & SCTP_DATA_FRAG_MASK)) {
		event->msg_flags |= MSG_EOR;
		return event;
	}

	sctp_ulpq_store_reasm(ulpq, event);
	if (!ulpq->pd_mode)
		retval = sctp_ulpq_retrieve_reassembled(ulpq);
	else {
		__u32 ctsn, ctsnap;

		/* Do not even bother unless this is the next tsn to
		 * be delivered.
		 */
		ctsn = event->tsn;
		ctsnap = sctp_tsnmap_get_ctsn(&ulpq->asoc->peer.tsn_map);
		if (TSN_lte(ctsn, ctsnap))
			retval = sctp_ulpq_retrieve_partial(ulpq);
	}

	return retval;
}