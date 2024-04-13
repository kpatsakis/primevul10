clusterip_hashfn(const struct sk_buff *skb,
		 const struct clusterip_config *config)
{
	const struct iphdr *iph = ip_hdr(skb);
	unsigned long hashval;
	u_int16_t sport = 0, dport = 0;
	int poff;

	poff = proto_ports_offset(iph->protocol);
	if (poff >= 0) {
		const u_int16_t *ports;
		u16 _ports[2];

		ports = skb_header_pointer(skb, iph->ihl * 4 + poff, 4, _ports);
		if (ports) {
			sport = ports[0];
			dport = ports[1];
		}
	} else {
		if (net_ratelimit())
			pr_info("unknown protocol %u\n", iph->protocol);
	}

	switch (config->hash_mode) {
	case CLUSTERIP_HASHMODE_SIP:
		hashval = jhash_1word(ntohl(iph->saddr),
				      config->hash_initval);
		break;
	case CLUSTERIP_HASHMODE_SIP_SPT:
		hashval = jhash_2words(ntohl(iph->saddr), sport,
				       config->hash_initval);
		break;
	case CLUSTERIP_HASHMODE_SIP_SPT_DPT:
		hashval = jhash_3words(ntohl(iph->saddr), sport, dport,
				       config->hash_initval);
		break;
	default:
		/* to make gcc happy */
		hashval = 0;
		/* This cannot happen, unless the check function wasn't called
		 * at rule load time */
		pr_info("unknown mode %u\n", config->hash_mode);
		BUG();
		break;
	}

	/* node numbers are 1..n, not 0..n */
	return (((u64)hashval * config->num_total_nodes) >> 32) + 1;
}