static inline int nla_attr_size(int payload)
{
	return NLA_HDRLEN + payload;
}