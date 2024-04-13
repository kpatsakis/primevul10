static inline void rtmsg_iwinfo(struct net_device *	dev,
				char *			event,
				int			event_len)
{
	struct sk_buff *skb;
	int size = NLMSG_GOODSIZE;

	skb = alloc_skb(size, GFP_ATOMIC);
	if (!skb)
		return;

	if (rtnetlink_fill_iwinfo(skb, dev, RTM_NEWLINK,
				  event, event_len) < 0) {
		kfree_skb(skb);
		return;
	}
	NETLINK_CB(skb).dst_groups = RTMGRP_LINK;
	netlink_broadcast(rtnl, skb, 0, RTMGRP_LINK, GFP_ATOMIC);
}