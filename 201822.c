static int udp_encap_rcv(struct sock * sk, struct sk_buff *skb)
{
#ifndef CONFIG_XFRM
	return 1; 
#else
	struct udp_sock *up = udp_sk(sk);
  	struct udphdr *uh = skb->h.uh;
	struct iphdr *iph;
	int iphlen, len;
  
	__u8 *udpdata = (__u8 *)uh + sizeof(struct udphdr);
	__be32 *udpdata32 = (__be32 *)udpdata;
	__u16 encap_type = up->encap_type;

	/* if we're overly short, let UDP handle it */
	if (udpdata > skb->tail)
		return 1;

	/* if this is not encapsulated socket, then just return now */
	if (!encap_type)
		return 1;

	len = skb->tail - udpdata;

	switch (encap_type) {
	default:
	case UDP_ENCAP_ESPINUDP:
		/* Check if this is a keepalive packet.  If so, eat it. */
		if (len == 1 && udpdata[0] == 0xff) {
			return 0;
		} else if (len > sizeof(struct ip_esp_hdr) && udpdata32[0] != 0 ) {
			/* ESP Packet without Non-ESP header */
			len = sizeof(struct udphdr);
		} else
			/* Must be an IKE packet.. pass it through */
			return 1;
		break;
	case UDP_ENCAP_ESPINUDP_NON_IKE:
		/* Check if this is a keepalive packet.  If so, eat it. */
		if (len == 1 && udpdata[0] == 0xff) {
			return 0;
		} else if (len > 2 * sizeof(u32) + sizeof(struct ip_esp_hdr) &&
			   udpdata32[0] == 0 && udpdata32[1] == 0) {
			
			/* ESP Packet with Non-IKE marker */
			len = sizeof(struct udphdr) + 2 * sizeof(u32);
		} else
			/* Must be an IKE packet.. pass it through */
			return 1;
		break;
	}

	/* At this point we are sure that this is an ESPinUDP packet,
	 * so we need to remove 'len' bytes from the packet (the UDP
	 * header and optional ESP marker bytes) and then modify the
	 * protocol to ESP, and then call into the transform receiver.
	 */
	if (skb_cloned(skb) && pskb_expand_head(skb, 0, 0, GFP_ATOMIC))
		return 0;

	/* Now we can update and verify the packet length... */
	iph = skb->nh.iph;
	iphlen = iph->ihl << 2;
	iph->tot_len = htons(ntohs(iph->tot_len) - len);
	if (skb->len < iphlen + len) {
		/* packet is too small!?! */
		return 0;
	}

	/* pull the data buffer up to the ESP header and set the
	 * transport header to point to ESP.  Keep UDP on the stack
	 * for later.
	 */
	skb->h.raw = skb_pull(skb, len);

	/* modify the protocol (it's ESP!) */
	iph->protocol = IPPROTO_ESP;

	/* and let the caller know to send this into the ESP processor... */
	return -1;
#endif
}