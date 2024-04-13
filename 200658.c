static inline void nlmsg_free(struct sk_buff *skb)
{
	kfree_skb(skb);
}