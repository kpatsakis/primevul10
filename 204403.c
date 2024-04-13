static int sctp_ulpq_clear_pd(struct sctp_ulpq *ulpq)
{
	ulpq->pd_mode = 0;
	return sctp_clear_pd(ulpq->asoc->base.sk);
}