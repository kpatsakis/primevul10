static void __exit inet6_exit(void)
{
	/* First of all disallow new sockets creation. */
	sock_unregister(PF_INET6);
	/* Disallow any further netlink messages */
	rtnl_unregister_all(PF_INET6);

#ifdef CONFIG_SYSCTL
	ipv6_sysctl_unregister();
#endif
	udpv6_exit();
	udplitev6_exit();
	tcpv6_exit();

	/* Cleanup code parts. */
	ipv6_packet_cleanup();
	ipv6_frag_exit();
	ipv6_exthdrs_exit();
	addrconf_cleanup();
	ip6_flowlabel_cleanup();
	ip6_route_cleanup();
#ifdef CONFIG_PROC_FS

	/* Cleanup code parts. */
	if6_proc_exit();
	ipv6_misc_proc_exit();
	udplite6_proc_exit();
	raw6_proc_exit();
#endif
	ipv6_netfilter_fini();
	igmp6_cleanup();
	ndisc_cleanup();
	icmpv6_cleanup();
	rawv6_exit();

	unregister_pernet_subsys(&inet6_net_ops);
	cleanup_ipv6_mibs();
	proto_unregister(&rawv6_prot);
	proto_unregister(&udplitev6_prot);
	proto_unregister(&udpv6_prot);
	proto_unregister(&tcpv6_prot);
}