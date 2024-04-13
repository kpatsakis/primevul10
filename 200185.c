ipip6_tunnel_del_prl(struct ip_tunnel *t, struct ip_tunnel_prl *a)
{
	struct ip_tunnel_prl_entry *x, **p;
	int err = 0;

	write_lock(&ipip6_lock);

	if (a && a->addr != htonl(INADDR_ANY)) {
		for (p = &t->prl; *p; p = &(*p)->next) {
			if ((*p)->addr == a->addr) {
				x = *p;
				*p = x->next;
				kfree(x);
				t->prl_count--;
				goto out;
			}
		}
		err = -ENXIO;
	} else {
		while (t->prl) {
			x = t->prl;
			t->prl = t->prl->next;
			kfree(x);
			t->prl_count--;
		}
	}
out:
	write_unlock(&ipip6_lock);
	return 0;
}