int sctp_ulpq_tail_event(struct sctp_ulpq *ulpq, struct sctp_ulpevent *event)
{
	struct sock *sk = ulpq->asoc->base.sk;
	struct sk_buff_head *queue, *skb_list;
	struct sk_buff *skb = sctp_event2skb(event);
	int clear_pd = 0;

	skb_list = (struct sk_buff_head *) skb->prev;

	/* If the socket is just going to throw this away, do not
	 * even try to deliver it.
	 */
	if (sock_flag(sk, SOCK_DEAD) || (sk->sk_shutdown & RCV_SHUTDOWN))
		goto out_free;

	/* Check if the user wishes to receive this event.  */
	if (!sctp_ulpevent_is_enabled(event, &sctp_sk(sk)->subscribe))
		goto out_free;

	/* If we are in partial delivery mode, post to the lobby until
	 * partial delivery is cleared, unless, of course _this_ is
	 * the association the cause of the partial delivery.
	 */

	if (!sctp_sk(sk)->pd_mode) {
		queue = &sk->sk_receive_queue;
	} else if (ulpq->pd_mode) {
		if (event->msg_flags & MSG_NOTIFICATION)
		       	queue = &sctp_sk(sk)->pd_lobby;
		else {
			clear_pd = event->msg_flags & MSG_EOR;
			queue = &sk->sk_receive_queue;
		}
	} else
		queue = &sctp_sk(sk)->pd_lobby;


	/* If we are harvesting multiple skbs they will be
	 * collected on a list.
	 */
	if (skb_list)
		sctp_skb_list_tail(skb_list, queue);
	else
		__skb_queue_tail(queue, skb);

	/* Did we just complete partial delivery and need to get
	 * rolling again?  Move pending data to the receive
	 * queue.
	 */
	if (clear_pd)
		sctp_ulpq_clear_pd(ulpq);

	if (queue == &sk->sk_receive_queue)
		sk->sk_data_ready(sk, 0);
	return 1;

out_free:
	if (skb_list)
		sctp_queue_purge_ulpevents(skb_list);
	else
		sctp_ulpevent_free(event);

	return 0;
}