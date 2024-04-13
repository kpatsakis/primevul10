static __inline__ int __udp_checksum_complete(struct sk_buff *skb)
{
	return __skb_checksum_complete(skb);
}