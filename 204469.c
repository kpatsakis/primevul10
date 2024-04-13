static int inet6_net_init(struct net *net)
{
	int err = 0;

	net->ipv6.sysctl.bindv6only = 0;
	net->ipv6.sysctl.flush_delay = 0;
	net->ipv6.sysctl.ip6_rt_max_size = 4096;
	net->ipv6.sysctl.ip6_rt_gc_min_interval = HZ / 2;
	net->ipv6.sysctl.ip6_rt_gc_timeout = 60*HZ;
	net->ipv6.sysctl.ip6_rt_gc_interval = 30*HZ;
	net->ipv6.sysctl.ip6_rt_gc_elasticity = 9;
	net->ipv6.sysctl.ip6_rt_mtu_expires = 10*60*HZ;
	net->ipv6.sysctl.ip6_rt_min_advmss = IPV6_MIN_MTU - 20 - 40;
	net->ipv6.sysctl.icmpv6_time = 1*HZ;

#ifdef CONFIG_PROC_FS
	err = udp6_proc_init(net);
	if (err)
		goto out;
	err = tcp6_proc_init(net);
	if (err)
		goto proc_tcp6_fail;
	err = ac6_proc_init(net);
	if (err)
		goto proc_ac6_fail;
out:
#endif
	return err;

#ifdef CONFIG_PROC_FS
proc_ac6_fail:
	tcp6_proc_exit(net);
proc_tcp6_fail:
	udp6_proc_exit(net);
	goto out;
#endif
}