static int tc_dump_tclass(struct sk_buff *skb, struct netlink_callback *cb)
{
	int t;
	int s_t;
	struct net_device *dev;
	struct Qdisc *q;
	struct tcmsg *tcm = (struct tcmsg*)NLMSG_DATA(cb->nlh);
	struct qdisc_dump_args arg;

	if (cb->nlh->nlmsg_len < NLMSG_LENGTH(sizeof(*tcm)))
		return 0;
	if ((dev = dev_get_by_index(tcm->tcm_ifindex)) == NULL)
		return 0;

	s_t = cb->args[0];
	t = 0;

	read_lock_bh(&qdisc_tree_lock);
	list_for_each_entry(q, &dev->qdisc_list, list) {
		if (t < s_t || !q->ops->cl_ops ||
		    (tcm->tcm_parent &&
		     TC_H_MAJ(tcm->tcm_parent) != q->handle)) {
			t++;
			continue;
		}
		if (t > s_t)
			memset(&cb->args[1], 0, sizeof(cb->args)-sizeof(cb->args[0]));
		arg.w.fn = qdisc_class_dump;
		arg.skb = skb;
		arg.cb = cb;
		arg.w.stop  = 0;
		arg.w.skip = cb->args[1];
		arg.w.count = 0;
		q->ops->cl_ops->walk(q, &arg.w);
		cb->args[1] = arg.w.count;
		if (arg.w.stop)
			break;
		t++;
	}
	read_unlock_bh(&qdisc_tree_lock);

	cb->args[0] = t;

	dev_put(dev);
	return skb->len;
}