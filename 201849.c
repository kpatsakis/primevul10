static inline int udp_v6_get_port(struct sock *sk, unsigned short snum)
{
	return udp_get_port(sk, snum, ipv6_rcv_saddr_equal);
}