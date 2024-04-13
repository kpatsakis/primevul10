act_get_notify(u32 pid, struct nlmsghdr *n, struct tc_action *a, int event)
{
	struct sk_buff *skb;
	int err = 0;

	skb = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!skb)
		return -ENOBUFS;
	if (tca_get_fill(skb, a, pid, n->nlmsg_seq, 0, event, 0, 0) <= 0) {
		kfree_skb(skb);
		return -EINVAL;
	}
	err = netlink_unicast(rtnl, skb, pid, MSG_DONTWAIT);
	if (err > 0)
		err = 0;
	return err;
}