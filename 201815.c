static int udp_getsockopt(struct sock *sk, int level, int optname,
			  char __user *optval, int __user *optlen)
{
	if (level != SOL_UDP)
		return ip_getsockopt(sk, level, optname, optval, optlen);
	return do_udp_getsockopt(sk, level, optname, optval, optlen);
}