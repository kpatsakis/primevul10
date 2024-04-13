static int udp_push_pending_frames(struct sock *sk, struct udp_sock *up)
{
	struct inet_sock *inet = inet_sk(sk);
	struct flowi *fl = &inet->cork.fl;
	struct sk_buff *skb;
	struct udphdr *uh;
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
		/*
		 * Only one fragment on the socket.
		 */
		if (skb->ip_summed == CHECKSUM_PARTIAL) {
			skb->csum = offsetof(struct udphdr, check);
			uh->check = ~csum_tcpudp_magic(fl->fl4_src, fl->fl4_dst,
					up->len, IPPROTO_UDP, 0);
		} else {
			skb->csum = csum_partial((char *)uh,
					sizeof(struct udphdr), skb->csum);
			uh->check = csum_tcpudp_magic(fl->fl4_src, fl->fl4_dst,
					up->len, IPPROTO_UDP, skb->csum);
			if (uh->check == 0)
				uh->check = -1;
		}
	} else {
		unsigned int csum = 0;
		/*
		 * HW-checksum won't work as there are two or more 
		 * fragments on the socket so that all csums of sk_buffs
		 * should be together.
		 */
		if (skb->ip_summed == CHECKSUM_PARTIAL) {
			int offset = (unsigned char *)uh - skb->data;
			skb->csum = skb_checksum(skb, offset, skb->len - offset, 0);

			skb->ip_summed = CHECKSUM_NONE;
		} else {
			skb->csum = csum_partial((char *)uh,
					sizeof(struct udphdr), skb->csum);
		}

		skb_queue_walk(&sk->sk_write_queue, skb) {
			csum = csum_add(csum, skb->csum);
		}
		uh->check = csum_tcpudp_magic(fl->fl4_src, fl->fl4_dst,
				up->len, IPPROTO_UDP, csum);
		if (uh->check == 0)
			uh->check = -1;
	}
send:
	err = ip_push_pending_frames(sk);
out:
	up->len = 0;
	up->pending = 0;
	return err;
}