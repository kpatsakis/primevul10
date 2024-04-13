static inline int nla_put_msecs(struct sk_buff *skb, int attrtype,
				unsigned long jiffies)
{
	u64 tmp = jiffies_to_msecs(jiffies);
	return nla_put(skb, attrtype, sizeof(u64), &tmp);
}