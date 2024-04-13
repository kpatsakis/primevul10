int neigh_add(struct sk_buff *skb, struct nlmsghdr *nlh, void *arg)
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
		struct rtattr *lladdr_attr = nda[NDA_LLADDR - 1];
		struct rtattr *dst_attr = nda[NDA_DST - 1];
		int override = 1;
		struct neighbour *n;

		if (tbl->family != ndm->ndm_family)
			continue;
		read_unlock(&neigh_tbl_lock);

		err = -EINVAL;
		if (!dst_attr || RTA_PAYLOAD(dst_attr) < tbl->key_len)
			goto out_dev_put;

		if (ndm->ndm_flags & NTF_PROXY) {
			err = -ENOBUFS;
			if (pneigh_lookup(tbl, RTA_DATA(dst_attr), dev, 1))
				err = 0;
			goto out_dev_put;
		}

		err = -EINVAL;
		if (!dev)
			goto out;
		if (lladdr_attr && RTA_PAYLOAD(lladdr_attr) < dev->addr_len)
			goto out_dev_put;
	
		n = neigh_lookup(tbl, RTA_DATA(dst_attr), dev);
		if (n) {
			if (nlh->nlmsg_flags & NLM_F_EXCL) {
				err = -EEXIST;
				neigh_release(n);
				goto out_dev_put;
			}
			
			override = nlh->nlmsg_flags & NLM_F_REPLACE;
		} else if (!(nlh->nlmsg_flags & NLM_F_CREATE)) {
			err = -ENOENT;
			goto out_dev_put;
		} else {
			n = __neigh_lookup_errno(tbl, RTA_DATA(dst_attr), dev);
			if (IS_ERR(n)) {
				err = PTR_ERR(n);
				goto out_dev_put;
			}
		}

		err = neigh_update(n,
				   lladdr_attr ? RTA_DATA(lladdr_attr) : NULL,
				   ndm->ndm_state,
				   (override ? NEIGH_UPDATE_F_OVERRIDE : 0) |
				   NEIGH_UPDATE_F_ADMIN);

		neigh_release(n);
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