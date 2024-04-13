static inline int nlmsg_multicast(struct sock *sk, struct sk_buff *skb,
				  u32 pid, unsigned int group, gfp_t flags)
{
	int err;

	NETLINK_CB(skb).dst_group = group;

	err = netlink_broadcast(sk, skb, pid, group, flags);
	if (err > 0)
		err = 0;

	return err;
}