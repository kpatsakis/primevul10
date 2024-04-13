__ipip6_tunnel_locate_prl(struct ip_tunnel *t, __be32 addr)
{
	struct ip_tunnel_prl_entry *p = (struct ip_tunnel_prl_entry *)NULL;

	for (p = t->prl; p; p = p->next)
		if (p->addr == addr)
			break;
	return p;

}