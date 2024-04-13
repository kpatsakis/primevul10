int sctp_clear_pd(struct sock *sk)
{
	struct sctp_sock *sp = sctp_sk(sk);

	sp->pd_mode = 0;
	if (!skb_queue_empty(&sp->pd_lobby)) {
		struct list_head *list;
		sctp_skb_list_tail(&sp->pd_lobby, &sk->sk_receive_queue);
		list = (struct list_head *)&sctp_sk(sk)->pd_lobby;
		INIT_LIST_HEAD(list);
		return 1;
	}
	return 0;
}