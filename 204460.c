static int __init init_ipv6_mibs(void)
{
	if (snmp_mib_init((void **)ipv6_statistics,
			  sizeof(struct ipstats_mib)) < 0)
		goto err_ip_mib;
	if (snmp_mib_init((void **)icmpv6_statistics,
			  sizeof(struct icmpv6_mib)) < 0)
		goto err_icmp_mib;
	if (snmp_mib_init((void **)icmpv6msg_statistics,
			  sizeof(struct icmpv6msg_mib)) < 0)
		goto err_icmpmsg_mib;
	if (snmp_mib_init((void **)udp_stats_in6, sizeof (struct udp_mib)) < 0)
		goto err_udp_mib;
	if (snmp_mib_init((void **)udplite_stats_in6,
			  sizeof (struct udp_mib)) < 0)
		goto err_udplite_mib;
	return 0;

err_udplite_mib:
	snmp_mib_free((void **)udp_stats_in6);
err_udp_mib:
	snmp_mib_free((void **)icmpv6msg_statistics);
err_icmpmsg_mib:
	snmp_mib_free((void **)icmpv6_statistics);
err_icmp_mib:
	snmp_mib_free((void **)ipv6_statistics);
err_ip_mib:
	return -ENOMEM;

}