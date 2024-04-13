static inline int nla_parse_nested(struct nlattr *tb[], int maxtype,
				   struct nlattr *nla,
				   const struct nla_policy *policy)
{
	return nla_parse(tb, maxtype, nla_data(nla), nla_len(nla), policy);
}