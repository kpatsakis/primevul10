static inline int nla_put_u8(struct sk_buff *skb, int attrtype, u8 value)
{
	return nla_put(skb, attrtype, sizeof(u8), &value);
}