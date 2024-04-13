static void ip_encap(struct sk_buff *skb, u32 saddr, u32 daddr)
{
	struct iphdr *iph = (struct iphdr *)skb_push(skb,sizeof(struct iphdr));

	iph->version	= 	4;
	iph->tos	=	skb->nh.iph->tos;
	iph->ttl	=	skb->nh.iph->ttl;
	iph->frag_off	=	0;
	iph->daddr	=	daddr;
	iph->saddr	=	saddr;
	iph->protocol	=	IPPROTO_IPIP;
	iph->ihl	=	5;
	iph->tot_len	=	htons(skb->len);
	ip_select_ident(iph, skb->dst, NULL);
	ip_send_check(iph);

	skb->h.ipiph = skb->nh.iph;
	skb->nh.iph = iph;
	memset(&(IPCB(skb)->opt), 0, sizeof(IPCB(skb)->opt));
	nf_reset(skb);
}