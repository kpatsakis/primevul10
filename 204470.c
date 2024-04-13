int inet6_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;
	struct net *net = sock_net(sk);

	switch(cmd)
	{
	case SIOCGSTAMP:
		return sock_get_timestamp(sk, (struct timeval __user *)arg);

	case SIOCGSTAMPNS:
		return sock_get_timestampns(sk, (struct timespec __user *)arg);

	case SIOCADDRT:
	case SIOCDELRT:

		return(ipv6_route_ioctl(net, cmd, (void __user *)arg));

	case SIOCSIFADDR:
		return addrconf_add_ifaddr(net, (void __user *) arg);
	case SIOCDIFADDR:
		return addrconf_del_ifaddr(net, (void __user *) arg);
	case SIOCSIFDSTADDR:
		return addrconf_set_dstaddr(net, (void __user *) arg);
	default:
		if (!sk->sk_prot->ioctl)
			return -ENOIOCTLCMD;
		return sk->sk_prot->ioctl(sk, cmd, arg);
	}
	/*NOTREACHED*/
	return(0);
}