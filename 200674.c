static inline int nla_nest_compat_end(struct sk_buff *skb, struct nlattr *start)
{
	struct nlattr *nest = (void *)start + NLMSG_ALIGN(start->nla_len);

	start->nla_len = skb_tail_pointer(skb) - (unsigned char *)start;
	return nla_nest_end(skb, nest);
}