static inline int __nla_parse_nested_compat(struct nlattr *tb[], int maxtype,
					    struct nlattr *nla,
					    const struct nla_policy *policy,
					    int len)
{
	int nested_len = nla_len(nla) - NLA_ALIGN(len);

	if (nested_len < 0)
		return -EINVAL;
	if (nested_len >= nla_attr_size(0))
		return nla_parse(tb, maxtype, nla_data(nla) + NLA_ALIGN(len),
				 nested_len, policy);
	memset(tb, 0, sizeof(struct nlattr *) * (maxtype + 1));
	return 0;
}