static inline struct ip_tunnel **ipip6_bucket(struct sit_net *sitn,
		struct ip_tunnel *t)
{
	return __ipip6_bucket(sitn, &t->parms);
}