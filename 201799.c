static void udpv6_mcast_deliver(struct udphdr *uh,
				struct in6_addr *saddr, struct in6_addr *daddr,
				struct sk_buff *skb)
{
	struct sock *sk, *sk2;
	int dif;

	read_lock(&udp_hash_lock);
	sk = sk_head(&udp_hash[ntohs(uh->dest) & (UDP_HTABLE_SIZE - 1)]);
	dif = skb->dev->ifindex;
	sk = udp_v6_mcast_next(sk, uh->dest, daddr, uh->source, saddr, dif);
	if (!sk) {
		kfree_skb(skb);
		goto out;
	}

	sk2 = sk;
	while ((sk2 = udp_v6_mcast_next(sk_next(sk2), uh->dest, daddr,
					uh->source, saddr, dif))) {
		struct sk_buff *buff = skb_clone(skb, GFP_ATOMIC);
		if (buff)
			udpv6_queue_rcv_skb(sk2, buff);
	}
	udpv6_queue_rcv_skb(sk, skb);
out:
	read_unlock(&udp_hash_lock);
}