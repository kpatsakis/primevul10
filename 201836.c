static void udp_checksum_init(struct sk_buff *skb, struct udphdr *uh,
			     unsigned short ulen, __be32 saddr, __be32 daddr)
{
	if (uh->check == 0) {
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	} else if (skb->ip_summed == CHECKSUM_COMPLETE) {
		if (!udp_check(uh, ulen, saddr, daddr, skb->csum))
			skb->ip_summed = CHECKSUM_UNNECESSARY;
	}
	if (skb->ip_summed != CHECKSUM_UNNECESSARY)
		skb->csum = csum_tcpudp_nofold(saddr, daddr, ulen, IPPROTO_UDP, 0);
	/* Probably, we should checksum udp header (it should be in cache
	 * in any case) and data in tiny packets (< rx copybreak).
	 */
}