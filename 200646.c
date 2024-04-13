static inline __be32 nla_get_be32(struct nlattr *nla)
{
	return *(__be32 *) nla_data(nla);
}