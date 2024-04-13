static void cleanup_ipv6_mibs(void)
{
	snmp_mib_free((void **)ipv6_statistics);
	snmp_mib_free((void **)icmpv6_statistics);
	snmp_mib_free((void **)icmpv6msg_statistics);
	snmp_mib_free((void **)udp_stats_in6);
	snmp_mib_free((void **)udplite_stats_in6);
}