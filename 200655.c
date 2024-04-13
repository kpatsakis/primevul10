static inline u16 nla_get_u16(struct nlattr *nla)
{
	return *(u16 *) nla_data(nla);
}