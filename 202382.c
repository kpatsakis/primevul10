static int reg_vif_xmit(struct sk_buff *skb, struct net_device *dev)
{
	read_lock(&mrt_lock);
	((struct net_device_stats*)dev->priv)->tx_bytes += skb->len;
	((struct net_device_stats*)dev->priv)->tx_packets++;
	ipmr_cache_report(skb, reg_vif_num, IGMPMSG_WHOLEPKT);
	read_unlock(&mrt_lock);
	kfree_skb(skb);
	return 0;
}