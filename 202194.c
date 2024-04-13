static void inet6_prefix_notify(int event, struct inet6_dev *idev, 
			 struct prefix_info *pinfo)
{
	struct sk_buff *skb;
	int size = NLMSG_SPACE(sizeof(struct prefixmsg)+128);

	skb = alloc_skb(size, GFP_ATOMIC);
	if (!skb) {
		netlink_set_err(rtnl, 0, RTMGRP_IPV6_PREFIX, ENOBUFS);
		return;
	}
	if (inet6_fill_prefix(skb, idev, pinfo, current->pid, 0, event, 0) < 0) {
		kfree_skb(skb);
		netlink_set_err(rtnl, 0, RTMGRP_IPV6_PREFIX, EINVAL);
		return;
	}
	NETLINK_CB(skb).dst_groups = RTMGRP_IPV6_PREFIX;
	netlink_broadcast(rtnl, skb, 0, RTMGRP_IPV6_PREFIX, GFP_ATOMIC);
}