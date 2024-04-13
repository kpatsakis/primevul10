void rtmsg_ifinfo(int type, struct net_device *dev, unsigned change)
{
	struct sk_buff *skb;
	int size = NLMSG_SPACE(sizeof(struct ifinfomsg) +
			       sizeof(struct rtnl_link_ifmap) +
			       sizeof(struct rtnl_link_stats) + 128);

	skb = alloc_skb(size, GFP_KERNEL);
	if (!skb)
		return;

	if (rtnetlink_fill_ifinfo(skb, dev, type, current->pid, 0, change, 0) < 0) {
		kfree_skb(skb);
		return;
	}
	NETLINK_CB(skb).dst_groups = RTMGRP_LINK;
	netlink_broadcast(rtnl, skb, 0, RTMGRP_LINK, GFP_KERNEL);
}