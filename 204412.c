void sctp_ulpq_abort_pd(struct sctp_ulpq *ulpq, gfp_t gfp)
{
	struct sctp_ulpevent *ev = NULL;
	struct sock *sk;

	if (!ulpq->pd_mode)
		return;

	sk = ulpq->asoc->base.sk;
	if (sctp_ulpevent_type_enabled(SCTP_PARTIAL_DELIVERY_EVENT,
				       &sctp_sk(sk)->subscribe))
		ev = sctp_ulpevent_make_pdapi(ulpq->asoc,
					      SCTP_PARTIAL_DELIVERY_ABORTED,
					      gfp);
	if (ev)
		__skb_queue_tail(&sk->sk_receive_queue, sctp_event2skb(ev));

	/* If there is data waiting, send it up the socket now. */
	if (sctp_ulpq_clear_pd(ulpq) || ev)
		sk->sk_data_ready(sk, 0);
}