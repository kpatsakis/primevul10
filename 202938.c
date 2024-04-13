clusterip_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct ipt_clusterip_tgt_info *cipinfo = par->targinfo;
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	u_int32_t hash;

	/* don't need to clusterip_config_get() here, since refcount
	 * is only decremented by destroy() - and ip_tables guarantees
	 * that the ->target() function isn't called after ->destroy() */

	ct = nf_ct_get(skb, &ctinfo);
	if (ct == NULL)
		return NF_DROP;

	/* special case: ICMP error handling. conntrack distinguishes between
	 * error messages (RELATED) and information requests (see below) */
	if (ip_hdr(skb)->protocol == IPPROTO_ICMP &&
	    (ctinfo == IP_CT_RELATED ||
	     ctinfo == IP_CT_RELATED + IP_CT_IS_REPLY))
		return XT_CONTINUE;

	/* ip_conntrack_icmp guarantees us that we only have ICMP_ECHO,
	 * TIMESTAMP, INFO_REQUEST or ADDRESS type icmp packets from here
	 * on, which all have an ID field [relevant for hashing]. */

	hash = clusterip_hashfn(skb, cipinfo->config);

	switch (ctinfo) {
		case IP_CT_NEW:
			ct->mark = hash;
			break;
		case IP_CT_RELATED:
		case IP_CT_RELATED+IP_CT_IS_REPLY:
			/* FIXME: we don't handle expectations at the
			 * moment.  they can arrive on a different node than
			 * the master connection (e.g. FTP passive mode) */
		case IP_CT_ESTABLISHED:
		case IP_CT_ESTABLISHED+IP_CT_IS_REPLY:
			break;
		default:
			break;
	}

#ifdef DEBUG
	nf_ct_dump_tuple_ip(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
#endif
	pr_debug("hash=%u ct_hash=%u ", hash, ct->mark);
	if (!clusterip_responsible(cipinfo->config, hash)) {
		pr_debug("not responsible\n");
		return NF_DROP;
	}
	pr_debug("responsible\n");

	/* despite being received via linklayer multicast, this is
	 * actually a unicast IP packet. TCP doesn't like PACKET_MULTICAST */
	skb->pkt_type = PACKET_HOST;

	return XT_CONTINUE;
}