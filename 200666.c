static inline u32 nla_get_u32(struct nlattr *nla)
{
	return *(u32 *) nla_data(nla);
}