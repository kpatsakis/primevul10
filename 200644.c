static inline __le16 nla_get_le16(struct nlattr *nla)
{
	return *(__le16 *) nla_data(nla);
}