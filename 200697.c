static inline __be16 nla_get_be16(struct nlattr *nla)
{
	return *(__be16 *) nla_data(nla);
}