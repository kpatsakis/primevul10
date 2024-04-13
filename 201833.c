static inline int udpv6_queue_rcv_skb(struct sock * sk, struct sk_buff *skb)
{
	int rc;

	if (!xfrm6_policy_check(sk, XFRM_POLICY_IN, skb)) {
		kfree_skb(skb);
		return -1;
	}

	if (skb_checksum_complete(skb)) {
		UDP6_INC_STATS_BH(UDP_MIB_INERRORS);
		kfree_skb(skb);
		return 0;
	}

	if ((rc = sock_queue_rcv_skb(sk,skb)) < 0) {
		/* Note that an ENOMEM error is charged twice */
		if (rc == -ENOMEM)
			UDP6_INC_STATS_BH(UDP_MIB_RCVBUFERRORS);
		UDP6_INC_STATS_BH(UDP_MIB_INERRORS);
		kfree_skb(skb);
		return 0;
	}
	UDP6_INC_STATS_BH(UDP_MIB_INDATAGRAMS);
	return 0;
}