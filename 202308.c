void neigh_app_ns(struct neighbour *n)
{
	struct nlmsghdr  *nlh;
	int size = NLMSG_SPACE(sizeof(struct ndmsg) + 256);
	struct sk_buff *skb = alloc_skb(size, GFP_ATOMIC);

	if (!skb)
		return;

	if (neigh_fill_info(skb, n, 0, 0, RTM_GETNEIGH, 0) < 0) {
		kfree_skb(skb);
		return;
	}
	nlh			   = (struct nlmsghdr *)skb->data;
	nlh->nlmsg_flags	   = NLM_F_REQUEST;
	NETLINK_CB(skb).dst_groups = RTMGRP_NEIGH;
	netlink_broadcast(rtnl, skb, 0, RTMGRP_NEIGH, GFP_ATOMIC);
}