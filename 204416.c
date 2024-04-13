void sctp_ulpq_skip(struct sctp_ulpq *ulpq, __u16 sid, __u16 ssn)
{
	struct sctp_stream *in;

	/* Note: The stream ID must be verified before this routine.  */
	in  = &ulpq->asoc->ssnmap->in;

	/* Is this an old SSN?  If so ignore. */
	if (SSN_lt(ssn, sctp_ssn_peek(in, sid)))
		return;

	/* Mark that we are no longer expecting this SSN or lower. */
	sctp_ssn_skip(in, sid, ssn);

	/* Go find any other chunks that were waiting for
	 * ordering and deliver them if needed. 
	 */
	sctp_ulpq_reap_ordered(ulpq);
	return;
}