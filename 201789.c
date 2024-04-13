static int udp_setsockopt(struct sock *sk, int level, int optname,
			  char __user *optval, int optlen)
{
	if (level != SOL_UDP)
		return ip_setsockopt(sk, level, optname, optval, optlen);
	return do_udp_setsockopt(sk, level, optname, optval, optlen);
}