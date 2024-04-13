void inet6_ifinfo_notify(int event, struct inet6_dev *idev)
{
	struct sk_buff *skb;
	/* 128 bytes ?? */
	int size = NLMSG_SPACE(sizeof(struct ifinfomsg)+128);
	
	skb = alloc_skb(size, GFP_ATOMIC);
	if (!skb) {
		netlink_set_err(rtnl, 0, RTMGRP_IPV6_IFINFO, ENOBUFS);
		return;
	}
	if (inet6_fill_ifinfo(skb, idev, current->pid, 0, event, 0) < 0) {
		kfree_skb(skb);
		netlink_set_err(rtnl, 0, RTMGRP_IPV6_IFINFO, EINVAL);
		return;
	}
	NETLINK_CB(skb).dst_groups = RTMGRP_IPV6_IFINFO;
	netlink_broadcast(rtnl, skb, 0, RTMGRP_IPV6_IFINFO, GFP_ATOMIC);
}