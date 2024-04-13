static int inet6_dump_ifinfo(struct sk_buff *skb, struct netlink_callback *cb)
{
	int idx, err;
	int s_idx = cb->args[0];
	struct net_device *dev;
	struct inet6_dev *idev;

	read_lock(&dev_base_lock);
	for (dev=dev_base, idx=0; dev; dev = dev->next, idx++) {
		if (idx < s_idx)
			continue;
		if ((idev = in6_dev_get(dev)) == NULL)
			continue;
		err = inet6_fill_ifinfo(skb, idev, NETLINK_CB(cb->skb).pid, 
				cb->nlh->nlmsg_seq, RTM_NEWLINK, NLM_F_MULTI);
		in6_dev_put(idev);
		if (err <= 0)
			break;
	}
	read_unlock(&dev_base_lock);
	cb->args[0] = idx;

	return skb->len;
}