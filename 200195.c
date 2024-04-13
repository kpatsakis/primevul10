ipip6_tunnel_add_prl(struct ip_tunnel *t, struct ip_tunnel_prl *a, int chg)
{
	struct ip_tunnel_prl_entry *p;
	int err = 0;

	if (a->addr == htonl(INADDR_ANY))
		return -EINVAL;

	write_lock(&ipip6_lock);

	for (p = t->prl; p; p = p->next) {
		if (p->addr == a->addr) {
			if (chg)
				goto update;
			err = -EEXIST;
			goto out;
		}
	}

	if (chg) {
		err = -ENXIO;
		goto out;
	}

	p = kzalloc(sizeof(struct ip_tunnel_prl_entry), GFP_KERNEL);
	if (!p) {
		err = -ENOBUFS;
		goto out;
	}

	p->next = t->prl;
	t->prl = p;
	t->prl_count++;
update:
	p->addr = a->addr;
	p->flags = a->flags;
out:
	write_unlock(&ipip6_lock);
	return err;
}