struct sctp_ulpq *sctp_ulpq_init(struct sctp_ulpq *ulpq,
				 struct sctp_association *asoc)
{
	memset(ulpq, 0, sizeof(struct sctp_ulpq));

	ulpq->asoc = asoc;
	skb_queue_head_init(&ulpq->reasm);
	skb_queue_head_init(&ulpq->lobby);
	ulpq->pd_mode  = 0;
	ulpq->malloced = 0;

	return ulpq;
}