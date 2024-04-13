int inet6_sk_rebuild_header(struct sock *sk)
{
	int err;
	struct dst_entry *dst;
	struct ipv6_pinfo *np = inet6_sk(sk);

	dst = __sk_dst_check(sk, np->dst_cookie);

	if (dst == NULL) {
		struct inet_sock *inet = inet_sk(sk);
		struct in6_addr *final_p = NULL, final;
		struct flowi fl;

		memset(&fl, 0, sizeof(fl));
		fl.proto = sk->sk_protocol;
		ipv6_addr_copy(&fl.fl6_dst, &np->daddr);
		ipv6_addr_copy(&fl.fl6_src, &np->saddr);
		fl.fl6_flowlabel = np->flow_label;
		fl.oif = sk->sk_bound_dev_if;
		fl.fl_ip_dport = inet->dport;
		fl.fl_ip_sport = inet->sport;
		security_sk_classify_flow(sk, &fl);

		if (np->opt && np->opt->srcrt) {
			struct rt0_hdr *rt0 = (struct rt0_hdr *) np->opt->srcrt;
			ipv6_addr_copy(&final, &fl.fl6_dst);
			ipv6_addr_copy(&fl.fl6_dst, rt0->addr);
			final_p = &final;
		}

		err = ip6_dst_lookup(sk, &dst, &fl);
		if (err) {
			sk->sk_route_caps = 0;
			return err;
		}
		if (final_p)
			ipv6_addr_copy(&fl.fl6_dst, final_p);

		if ((err = xfrm_lookup(&dst, &fl, sk, 0)) < 0) {
			sk->sk_err_soft = -err;
			return err;
		}

		__ip6_dst_store(sk, dst, NULL, NULL);
	}

	return 0;
}