static int compat_udpv6_setsockopt(struct sock *sk, int level, int optname,
				   char __user *optval, int optlen)
{
	if (level != SOL_UDP)
		return compat_ipv6_setsockopt(sk, level, optname,
					      optval, optlen);
	return do_udpv6_setsockopt(sk, level, optname, optval, optlen);
}