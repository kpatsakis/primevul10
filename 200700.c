static inline int nla_get_flag(struct nlattr *nla)
{
	return !!nla;
}