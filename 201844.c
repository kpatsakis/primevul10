int udp_rcv(struct sk_buff *skb)
{
  	struct sock *sk;
  	struct udphdr *uh;
	unsigned short ulen;
	struct rtable *rt = (struct rtable*)skb->dst;
	__be32 saddr = skb->nh.iph->saddr;
	__be32 daddr = skb->nh.iph->daddr;
	int len = skb->len;

	/*
	 *	Validate the packet and the UDP length.
	 */
	if (!pskb_may_pull(skb, sizeof(struct udphdr)))
		goto no_header;

	uh = skb->h.uh;

	ulen = ntohs(uh->len);

	if (ulen > len || ulen < sizeof(*uh))
		goto short_packet;

	if (pskb_trim_rcsum(skb, ulen))
		goto short_packet;

	udp_checksum_init(skb, uh, ulen, saddr, daddr);

	if(rt->rt_flags & (RTCF_BROADCAST|RTCF_MULTICAST))
		return udp_v4_mcast_deliver(skb, uh, saddr, daddr);

	sk = udp_v4_lookup(saddr, uh->source, daddr, uh->dest, skb->dev->ifindex);

	if (sk != NULL) {
		int ret = udp_queue_rcv_skb(sk, skb);
		sock_put(sk);

		/* a return value > 0 means to resubmit the input, but
		 * it it wants the return to be -protocol, or 0
		 */
		if (ret > 0)
			return -ret;
		return 0;
	}

	if (!xfrm4_policy_check(NULL, XFRM_POLICY_IN, skb))
		goto drop;
	nf_reset(skb);

	/* No socket. Drop packet silently, if checksum is wrong */
	if (udp_checksum_complete(skb))
		goto csum_error;

	UDP_INC_STATS_BH(UDP_MIB_NOPORTS);
	icmp_send(skb, ICMP_DEST_UNREACH, ICMP_PORT_UNREACH, 0);

	/*
	 * Hmm.  We got an UDP packet to a port to which we
	 * don't wanna listen.  Ignore it.
	 */
	kfree_skb(skb);
	return(0);

short_packet:
	LIMIT_NETDEBUG(KERN_DEBUG "UDP: short packet: From %u.%u.%u.%u:%u %d/%d to %u.%u.%u.%u:%u\n",
		       NIPQUAD(saddr),
		       ntohs(uh->source),
		       ulen,
		       len,
		       NIPQUAD(daddr),
		       ntohs(uh->dest));
no_header:
	UDP_INC_STATS_BH(UDP_MIB_INERRORS);
	kfree_skb(skb);
	return(0);

csum_error:
	/* 
	 * RFC1122: OK.  Discards the bad packet silently (as far as 
	 * the network is concerned, anyway) as per 4.1.3.4 (MUST). 
	 */
	LIMIT_NETDEBUG(KERN_DEBUG "UDP: bad checksum. From %d.%d.%d.%d:%d to %d.%d.%d.%d:%d ulen %d\n",
		       NIPQUAD(saddr),
		       ntohs(uh->source),
		       NIPQUAD(daddr),
		       ntohs(uh->dest),
		       ulen);
drop:
	UDP_INC_STATS_BH(UDP_MIB_INERRORS);
	kfree_skb(skb);
	return(0);
}