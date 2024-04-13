void sctp_ulpq_partial_delivery(struct sctp_ulpq *ulpq,
				struct sctp_chunk *chunk,
				gfp_t gfp)
{
	struct sctp_ulpevent *event;
	struct sctp_association *asoc;

	asoc = ulpq->asoc;

	/* Are we already in partial delivery mode?  */
	if (!sctp_sk(asoc->base.sk)->pd_mode) {

		/* Is partial delivery possible?  */
		event = sctp_ulpq_retrieve_first(ulpq);
		/* Send event to the ULP.   */
		if (event) {
			sctp_ulpq_tail_event(ulpq, event);
			sctp_sk(asoc->base.sk)->pd_mode = 1;
			ulpq->pd_mode = 1;
			return;
		}
	}
}