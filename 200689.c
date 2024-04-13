static inline u8 nla_get_u8(struct nlattr *nla)
{
	return *(u8 *) nla_data(nla);
}