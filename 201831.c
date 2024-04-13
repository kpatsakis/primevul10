static int udp_v6_push_pending_frames(struct sock *sk, struct udp_sock *up)
{
	struct sk_buff *skb;
	struct udphdr *uh;
	struct inet_sock *inet = inet_sk(sk);
	struct flowi *fl = &inet->cork.fl;
	int err = 0;

	/* Grab the skbuff where UDP header space exists. */
	if ((skb = skb_peek(&sk->sk_write_queue)) == NULL)
		goto out;

	/*
	 * Create a UDP header
	 */
	uh = skb->h.uh;
	uh->source = fl->fl_ip_sport;
	uh->dest = fl->fl_ip_dport;
	uh->len = htons(up->len);
	uh->check = 0;

	if (sk->sk_no_check == UDP_CSUM_NOXMIT) {
		skb->ip_summed = CHECKSUM_NONE;
		goto send;
	}

	if (skb_queue_len(&sk->sk_write_queue) == 1) {
		skb->csum = csum_partial((char *)uh,
				sizeof(struct udphdr), skb->csum);
		uh->check = csum_ipv6_magic(&fl->fl6_src,
					    &fl->fl6_dst,
					    up->len, fl->proto, skb->csum);
	} else {
		u32 tmp_csum = 0;

		skb_queue_walk(&sk->sk_write_queue, skb) {
			tmp_csum = csum_add(tmp_csum, skb->csum);
		}
		tmp_csum = csum_partial((char *)uh,
				sizeof(struct udphdr), tmp_csum);
                tmp_csum = csum_ipv6_magic(&fl->fl6_src,
					   &fl->fl6_dst,
					   up->len, fl->proto, tmp_csum);
                uh->check = tmp_csum;

	}
	if (uh->check == 0)
		uh->check = -1;

send:
	err = ip6_push_pending_frames(sk);
out:
	up->len = 0;
	up->pending = 0;
	return err;
}