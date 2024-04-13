static inline int nla_padlen(int payload)
{
	return nla_total_size(payload) - nla_attr_size(payload);
}