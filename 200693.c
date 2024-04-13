static inline void nlmsg_trim(struct sk_buff *skb, const void *mark)
{
	if (mark)
		skb_trim(skb, (unsigned char *) mark - skb->data);
}