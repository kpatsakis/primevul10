static int ipv6_gso_send_check(struct sk_buff *skb)
{
	struct ipv6hdr *ipv6h;
	struct inet6_protocol *ops;
	int err = -EINVAL;

	if (unlikely(!pskb_may_pull(skb, sizeof(*ipv6h))))
		goto out;

	ipv6h = ipv6_hdr(skb);
	__skb_pull(skb, sizeof(*ipv6h));
	err = -EPROTONOSUPPORT;

	rcu_read_lock();
	ops = ipv6_gso_pull_exthdrs(skb, ipv6h->nexthdr);
	if (likely(ops && ops->gso_send_check)) {
		skb_reset_transport_header(skb);
		err = ops->gso_send_check(skb);
	}
	rcu_read_unlock();

out:
	return err;
}