static int tc_get_qdisc(struct sk_buff *skb, struct nlmsghdr *n, void *arg)
{
	struct tcmsg *tcm = NLMSG_DATA(n);
	struct rtattr **tca = arg;
	struct net_device *dev;
	u32 clid = tcm->tcm_parent;
	struct Qdisc *q = NULL;
	struct Qdisc *p = NULL;
	int err;

	if ((dev = __dev_get_by_index(tcm->tcm_ifindex)) == NULL)
		return -ENODEV;

	if (clid) {
		if (clid != TC_H_ROOT) {
			if (TC_H_MAJ(clid) != TC_H_MAJ(TC_H_INGRESS)) {
				if ((p = qdisc_lookup(dev, TC_H_MAJ(clid))) == NULL)
					return -ENOENT;
				q = qdisc_leaf(p, clid);
			} else { /* ingress */
				q = dev->qdisc_ingress;
                        }
		} else {
			q = dev->qdisc_sleeping;
		}
		if (!q)
			return -ENOENT;

		if (tcm->tcm_handle && q->handle != tcm->tcm_handle)
			return -EINVAL;
	} else {
		if ((q = qdisc_lookup(dev, tcm->tcm_handle)) == NULL)
			return -ENOENT;
	}

	if (tca[TCA_KIND-1] && rtattr_strcmp(tca[TCA_KIND-1], q->ops->id))
		return -EINVAL;

	if (n->nlmsg_type == RTM_DELQDISC) {
		if (!clid)
			return -EINVAL;
		if (q->handle == 0)
			return -ENOENT;
		if ((err = qdisc_graft(dev, p, clid, NULL, &q)) != 0)
			return err;
		if (q) {
			qdisc_notify(skb, n, clid, q, NULL);
			spin_lock_bh(&dev->queue_lock);
			qdisc_destroy(q);
			spin_unlock_bh(&dev->queue_lock);
		}
	} else {
		qdisc_notify(skb, n, clid, NULL, q);
	}
	return 0;
}