int neigh_delete(struct sk_buff *skb, struct nlmsghdr *nlh, void *arg)
{
	struct ndmsg *ndm = NLMSG_DATA(nlh);
	struct rtattr **nda = arg;
	struct neigh_table *tbl;
	struct net_device *dev = NULL;
	int err = -ENODEV;

	if (ndm->ndm_ifindex &&
	    (dev = dev_get_by_index(ndm->ndm_ifindex)) == NULL)
		goto out;

	read_lock(&neigh_tbl_lock);
	for (tbl = neigh_tables; tbl; tbl = tbl->next) {
		struct rtattr *dst_attr = nda[NDA_DST - 1];
		struct neighbour *n;

		if (tbl->family != ndm->ndm_family)
			continue;
		read_unlock(&neigh_tbl_lock);

		err = -EINVAL;
		if (!dst_attr || RTA_PAYLOAD(dst_attr) < tbl->key_len)
			goto out_dev_put;

		if (ndm->ndm_flags & NTF_PROXY) {
			err = pneigh_delete(tbl, RTA_DATA(dst_attr), dev);
			goto out_dev_put;
		}

		if (!dev)
			goto out;

		n = neigh_lookup(tbl, RTA_DATA(dst_attr), dev);
		if (n) {
			err = neigh_update(n, NULL, NUD_FAILED, 
					   NEIGH_UPDATE_F_OVERRIDE|
					   NEIGH_UPDATE_F_ADMIN);
			neigh_release(n);
		}
		goto out_dev_put;
	}
	read_unlock(&neigh_tbl_lock);
	err = -EADDRNOTAVAIL;
out_dev_put:
	if (dev)
		dev_put(dev);
out:
	return err;
}