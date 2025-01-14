static int tc_fill_tclass(struct sk_buff *skb, struct Qdisc *q,
			  unsigned long cl,
			  u32 pid, u32 seq, u16 flags, int event)
{
	struct tcmsg *tcm;
	struct nlmsghdr  *nlh;
	unsigned char	 *b = skb->tail;
	struct gnet_dump d;
	struct Qdisc_class_ops *cl_ops = q->ops->cl_ops;

	nlh = NLMSG_NEW(skb, pid, seq, event, sizeof(*tcm), flags);
	tcm = NLMSG_DATA(nlh);
	tcm->tcm_family = AF_UNSPEC;
	tcm->tcm_ifindex = q->dev->ifindex;
	tcm->tcm_parent = q->handle;
	tcm->tcm_handle = q->handle;
	tcm->tcm_info = 0;
	RTA_PUT(skb, TCA_KIND, IFNAMSIZ, q->ops->id);
	if (cl_ops->dump && cl_ops->dump(q, cl, skb, tcm) < 0)
		goto rtattr_failure;

	if (gnet_stats_start_copy_compat(skb, TCA_STATS2, TCA_STATS,
			TCA_XSTATS, q->stats_lock, &d) < 0)
		goto rtattr_failure;

	if (cl_ops->dump_stats && cl_ops->dump_stats(q, cl, &d) < 0)
		goto rtattr_failure;

	if (gnet_stats_finish_copy(&d) < 0)
		goto rtattr_failure;

	nlh->nlmsg_len = skb->tail - b;
	return skb->len;

nlmsg_failure:
rtattr_failure:
	skb_trim(skb, b - skb->data);
	return -1;
}