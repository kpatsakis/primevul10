static inline int nla_put_u64(struct sk_buff *skb, int attrtype, u64 value)
{
	return nla_put(skb, attrtype, sizeof(u64), &value);
}