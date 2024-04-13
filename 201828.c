static inline int udp_lport_inuse(u16 num)
{
	struct sock *sk;
	struct hlist_node *node;

	sk_for_each(sk, node, &udp_hash[num & (UDP_HTABLE_SIZE - 1)])
		if (inet_sk(sk)->num == num)
			return 1;
	return 0;
}