static int compat_udp_getsockopt(struct sock *sk, int level, int optname,
				 char __user *optval, int __user *optlen)
{
	if (level != SOL_UDP)
		return compat_ip_getsockopt(sk, level, optname, optval, optlen);
	return do_udp_getsockopt(sk, level, optname, optval, optlen);
}