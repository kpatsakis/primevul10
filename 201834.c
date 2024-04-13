static int udp_queue_rcv_skb(struct sock * sk, struct sk_buff *skb)
{
	struct udp_sock *up = udp_sk(sk);
	int rc;

	/*
	 *	Charge it to the socket, dropping if the queue is full.
	 */
	if (!xfrm4_policy_check(sk, XFRM_POLICY_IN, skb)) {
		kfree_skb(skb);
		return -1;
	}
	nf_reset(skb);

	if (up->encap_type) {
		/*
		 * This is an encapsulation socket, so let's see if this is
		 * an encapsulated packet.
		 * If it's a keepalive packet, then just eat it.
		 * If it's an encapsulateed packet, then pass it to the
		 * IPsec xfrm input and return the response
		 * appropriately.  Otherwise, just fall through and
		 * pass this up the UDP socket.
		 */
		int ret;

		ret = udp_encap_rcv(sk, skb);
		if (ret == 0) {
			/* Eat the packet .. */
			kfree_skb(skb);
			return 0;
		}
		if (ret < 0) {
			/* process the ESP packet */
			ret = xfrm4_rcv_encap(skb, up->encap_type);
			UDP_INC_STATS_BH(UDP_MIB_INDATAGRAMS);
			return -ret;
		}
		/* FALLTHROUGH -- it's a UDP Packet */
	}

	if (sk->sk_filter && skb->ip_summed != CHECKSUM_UNNECESSARY) {
		if (__udp_checksum_complete(skb)) {
			UDP_INC_STATS_BH(UDP_MIB_INERRORS);
			kfree_skb(skb);
			return -1;
		}
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	}

	if ((rc = sock_queue_rcv_skb(sk,skb)) < 0) {
		/* Note that an ENOMEM error is charged twice */
		if (rc == -ENOMEM)
			UDP_INC_STATS_BH(UDP_MIB_RCVBUFERRORS);
		UDP_INC_STATS_BH(UDP_MIB_INERRORS);
		kfree_skb(skb);
		return -1;
	}
	UDP_INC_STATS_BH(UDP_MIB_INDATAGRAMS);
	return 0;
}