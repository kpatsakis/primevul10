static int udpv6_getsockopt(struct sock *sk, int level, int optname,
			  char __user *optval, int __user *optlen)
{
	if (level != SOL_UDP)
		return ipv6_getsockopt(sk, level, optname, optval, optlen);
	return do_udpv6_getsockopt(sk, level, optname, optval, optlen);
}