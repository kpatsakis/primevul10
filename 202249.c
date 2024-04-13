int pim_rcv_v1(struct sk_buff * skb)
{
	struct igmphdr *pim;
	struct iphdr   *encap;
	struct net_device  *reg_dev = NULL;

	if (!pskb_may_pull(skb, sizeof(*pim) + sizeof(*encap))) 
		goto drop;

	pim = (struct igmphdr*)skb->h.raw;

        if (!mroute_do_pim ||
	    skb->len < sizeof(*pim) + sizeof(*encap) ||
	    pim->group != PIM_V1_VERSION || pim->code != PIM_V1_REGISTER) 
		goto drop;

	encap = (struct iphdr*)(skb->h.raw + sizeof(struct igmphdr));
	/*
	   Check that:
	   a. packet is really destinted to a multicast group
	   b. packet is not a NULL-REGISTER
	   c. packet is not truncated
	 */
	if (!MULTICAST(encap->daddr) ||
	    encap->tot_len == 0 ||
	    ntohs(encap->tot_len) + sizeof(*pim) > skb->len) 
		goto drop;

	read_lock(&mrt_lock);
	if (reg_vif_num >= 0)
		reg_dev = vif_table[reg_vif_num].dev;
	if (reg_dev)
		dev_hold(reg_dev);
	read_unlock(&mrt_lock);

	if (reg_dev == NULL) 
		goto drop;

	skb->mac.raw = skb->nh.raw;
	skb_pull(skb, (u8*)encap - skb->data);
	skb->nh.iph = (struct iphdr *)skb->data;
	skb->dev = reg_dev;
	memset(&(IPCB(skb)->opt), 0, sizeof(struct ip_options));
	skb->protocol = htons(ETH_P_IP);
	skb->ip_summed = 0;
	skb->pkt_type = PACKET_HOST;
	dst_release(skb->dst);
	skb->dst = NULL;
	((struct net_device_stats*)reg_dev->priv)->rx_bytes += skb->len;
	((struct net_device_stats*)reg_dev->priv)->rx_packets++;
	nf_reset(skb);
	netif_rx(skb);
	dev_put(reg_dev);
	return 0;
 drop:
	kfree_skb(skb);
	return 0;
}