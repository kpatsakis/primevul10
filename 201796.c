static unsigned short udp_check(struct udphdr *uh, int len, __be32 saddr, __be32 daddr, unsigned long base)
{
	return(csum_tcpudp_magic(saddr, daddr, len, IPPROTO_UDP, base));
}