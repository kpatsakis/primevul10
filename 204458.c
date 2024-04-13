int ipv6_opt_accepted(struct sock *sk, struct sk_buff *skb)
{
	struct ipv6_pinfo *np = inet6_sk(sk);
	struct inet6_skb_parm *opt = IP6CB(skb);

	if (np->rxopt.all) {
		if ((opt->hop && (np->rxopt.bits.hopopts ||
				  np->rxopt.bits.ohopopts)) ||
		    ((IPV6_FLOWINFO_MASK &
		      *(__be32 *)skb_network_header(skb)) &&
		     np->rxopt.bits.rxflow) ||
		    (opt->srcrt && (np->rxopt.bits.srcrt ||
		     np->rxopt.bits.osrcrt)) ||
		    ((opt->dst1 || opt->dst0) &&
		     (np->rxopt.bits.dstopts || np->rxopt.bits.odstopts)))
			return 1;
	}
	return 0;
}