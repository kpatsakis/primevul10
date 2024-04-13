static inline int nla_total_size(int payload)
{
	return NLA_ALIGN(nla_attr_size(payload));
}