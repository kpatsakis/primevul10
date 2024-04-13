void sctp_ulpq_free(struct sctp_ulpq *ulpq)
{
	sctp_ulpq_flush(ulpq);
	if (ulpq->malloced)
		kfree(ulpq);
}