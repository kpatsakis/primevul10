static inline struct nlattr *nla_nest_compat_start(struct sk_buff *skb,
						   int attrtype, int attrlen,
						   const void *data)
{
	struct nlattr *start = (struct nlattr *)skb_tail_pointer(skb);

	if (nla_put(skb, attrtype, attrlen, data) < 0)
		return NULL;
	if (nla_nest_start(skb, attrtype) == NULL) {
		nlmsg_trim(skb, start);
		return NULL;
	}
	return start;
}