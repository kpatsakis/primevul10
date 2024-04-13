int ipv6_rcv_saddr_equal(const struct sock *sk, const struct sock *sk2)
{
	const struct in6_addr *sk_rcv_saddr6 = &inet6_sk(sk)->rcv_saddr;
	const struct in6_addr *sk2_rcv_saddr6 = tcp_v6_rcv_saddr(sk2);
	u32 sk_rcv_saddr = inet_sk(sk)->rcv_saddr;
	u32 sk2_rcv_saddr = tcp_v4_rcv_saddr(sk2);
	int sk_ipv6only = ipv6_only_sock(sk);
	int sk2_ipv6only = tcp_v6_ipv6only(sk2);
	int addr_type = ipv6_addr_type(sk_rcv_saddr6);
	int addr_type2 = sk2_rcv_saddr6 ? ipv6_addr_type(sk2_rcv_saddr6) : IPV6_ADDR_MAPPED;

	if (!sk2_rcv_saddr && !sk_ipv6only)
		return 1;

	if (addr_type2 == IPV6_ADDR_ANY &&
	    !(sk2_ipv6only && addr_type == IPV6_ADDR_MAPPED))
		return 1;

	if (addr_type == IPV6_ADDR_ANY &&
	    !(sk_ipv6only && addr_type2 == IPV6_ADDR_MAPPED))
		return 1;

	if (sk2_rcv_saddr6 &&
	    ipv6_addr_equal(sk_rcv_saddr6, sk2_rcv_saddr6))
		return 1;

	if (addr_type == IPV6_ADDR_MAPPED &&
	    !sk2_ipv6only &&
	    (!sk2_rcv_saddr || !sk_rcv_saddr || sk_rcv_saddr == sk2_rcv_saddr))
		return 1;

	return 0;
}