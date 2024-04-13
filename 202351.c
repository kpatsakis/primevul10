int neightbl_dump_info(struct sk_buff *skb, struct netlink_callback *cb)
{
	int idx, family;
	int s_idx = cb->args[0];
	struct neigh_table *tbl;

	family = ((struct rtgenmsg *)NLMSG_DATA(cb->nlh))->rtgen_family;

	read_lock(&neigh_tbl_lock);
	for (tbl = neigh_tables, idx = 0; tbl; tbl = tbl->next) {
		struct neigh_parms *p;

		if (idx < s_idx || (family && tbl->family != family))
			continue;

		if (neightbl_fill_info(tbl, skb, cb) <= 0)
			break;

		for (++idx, p = tbl->parms.next; p; p = p->next, idx++) {
			if (idx < s_idx)
				continue;

			if (neightbl_fill_param_info(tbl, p, skb, cb) <= 0)
				goto out;
		}

	}
out:
	read_unlock(&neigh_tbl_lock);
	cb->args[0] = idx;

	return skb->len;
}