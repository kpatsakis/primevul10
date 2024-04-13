static int ipmr_cache_report(struct sk_buff *pkt, vifi_t vifi, int assert)
{
	struct sk_buff *skb;
	int ihl = pkt->nh.iph->ihl<<2;
	struct igmphdr *igmp;
	struct igmpmsg *msg;
	int ret;

#ifdef CONFIG_IP_PIMSM
	if (assert == IGMPMSG_WHOLEPKT)
		skb = skb_realloc_headroom(pkt, sizeof(struct iphdr));
	else
#endif
		skb = alloc_skb(128, GFP_ATOMIC);

	if(!skb)
		return -ENOBUFS;

#ifdef CONFIG_IP_PIMSM
	if (assert == IGMPMSG_WHOLEPKT) {
		/* Ugly, but we have no choice with this interface.
		   Duplicate old header, fix ihl, length etc.
		   And all this only to mangle msg->im_msgtype and
		   to set msg->im_mbz to "mbz" :-)
		 */
		msg = (struct igmpmsg*)skb_push(skb, sizeof(struct iphdr));
		skb->nh.raw = skb->h.raw = (u8*)msg;
		memcpy(msg, pkt->nh.raw, sizeof(struct iphdr));
		msg->im_msgtype = IGMPMSG_WHOLEPKT;
		msg->im_mbz = 0;
 		msg->im_vif = reg_vif_num;
		skb->nh.iph->ihl = sizeof(struct iphdr) >> 2;
		skb->nh.iph->tot_len = htons(ntohs(pkt->nh.iph->tot_len) + sizeof(struct iphdr));
	} else 
#endif
	{	
		
	/*
	 *	Copy the IP header
	 */

	skb->nh.iph = (struct iphdr *)skb_put(skb, ihl);
	memcpy(skb->data,pkt->data,ihl);
	skb->nh.iph->protocol = 0;			/* Flag to the kernel this is a route add */
	msg = (struct igmpmsg*)skb->nh.iph;
	msg->im_vif = vifi;
	skb->dst = dst_clone(pkt->dst);

	/*
	 *	Add our header
	 */

	igmp=(struct igmphdr *)skb_put(skb,sizeof(struct igmphdr));
	igmp->type	=
	msg->im_msgtype = assert;
	igmp->code 	=	0;
	skb->nh.iph->tot_len=htons(skb->len);			/* Fix the length */
	skb->h.raw = skb->nh.raw;
        }

	if (mroute_socket == NULL) {
		kfree_skb(skb);
		return -EINVAL;
	}

	/*
	 *	Deliver to mrouted
	 */
	if ((ret=sock_queue_rcv_skb(mroute_socket,skb))<0) {
		if (net_ratelimit())
			printk(KERN_WARNING "mroute: pending queue full, dropping entries.\n");
		kfree_skb(skb);
	}

	return ret;
}