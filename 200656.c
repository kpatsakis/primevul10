static inline struct nlmsghdr *__nlmsg_put(struct sk_buff *skb, u32 pid,
					   u32 seq, int type, int payload,
					   int flags)
{
	struct nlmsghdr *nlh;

	nlh = (struct nlmsghdr *) skb_put(skb, nlmsg_total_size(payload));
	nlh->nlmsg_type = type;
	nlh->nlmsg_len = nlmsg_msg_size(payload);
	nlh->nlmsg_flags = flags;
	nlh->nlmsg_pid = pid;
	nlh->nlmsg_seq = seq;

	memset((unsigned char *) nlmsg_data(nlh) + payload, 0,
	       nlmsg_padlen(payload));

	return nlh;
}