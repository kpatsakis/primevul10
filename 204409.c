static struct sctp_ulpevent *sctp_ulpq_order(struct sctp_ulpq *ulpq,
					     struct sctp_ulpevent *event)
{
	__u16 sid, ssn;
	struct sctp_stream *in;

	/* Check if this message needs ordering.  */
	if (SCTP_DATA_UNORDERED & event->msg_flags)
		return event;

	/* Note: The stream ID must be verified before this routine.  */
	sid = event->stream;
	ssn = event->ssn;
	in  = &ulpq->asoc->ssnmap->in;

	/* Is this the expected SSN for this stream ID?  */
	if (ssn != sctp_ssn_peek(in, sid)) {
		/* We've received something out of order, so find where it
		 * needs to be placed.  We order by stream and then by SSN.
		 */
		sctp_ulpq_store_ordered(ulpq, event);
		return NULL;
	}

	/* Mark that the next chunk has been found.  */
	sctp_ssn_next(in, sid);

	/* Go find any other chunks that were waiting for
	 * ordering.
	 */
	sctp_ulpq_retrieve_ordered(ulpq, event);

	return event;
}