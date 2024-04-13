static inline int nlmsg_unicast(struct sock *sk, struct sk_buff *skb, u32 pid)
{
	int err;

	err = netlink_unicast(sk, skb, pid, MSG_DONTWAIT);
	if (err > 0)
		err = 0;

	return err;
}