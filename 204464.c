static struct sk_buff *ipv6_gso_segment(struct sk_buff *skb, int features)
{
	struct sk_buff *segs = ERR_PTR(-EINVAL);
	struct ipv6hdr *ipv6h;
	struct inet6_protocol *ops;

	if (!(features & NETIF_F_V6_CSUM))
		features &= ~NETIF_F_SG;

	if (unlikely(skb_shinfo(skb)->gso_type &
		     ~(SKB_GSO_UDP |
		       SKB_GSO_DODGY |
		       SKB_GSO_TCP_ECN |
		       SKB_GSO_TCPV6 |
		       0)))
		goto out;

	if (unlikely(!pskb_may_pull(skb, sizeof(*ipv6h))))
		goto out;

	ipv6h = ipv6_hdr(skb);
	__skb_pull(skb, sizeof(*ipv6h));
	segs = ERR_PTR(-EPROTONOSUPPORT);

	rcu_read_lock();
	ops = ipv6_gso_pull_exthdrs(skb, ipv6h->nexthdr);
	if (likely(ops && ops->gso_segment)) {
		skb_reset_transport_header(skb);
		segs = ops->gso_segment(skb, features);
	}
	rcu_read_unlock();

	if (unlikely(IS_ERR(segs)))
		goto out;

	for (skb = segs; skb; skb = skb->next) {
		ipv6h = ipv6_hdr(skb);
		ipv6h->payload_len = htons(skb->len - skb->mac_len -
					   sizeof(*ipv6h));
	}

out:
	return segs;
}