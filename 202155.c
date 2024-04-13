static int cbq_graft(struct Qdisc *sch, unsigned long arg, struct Qdisc *new,
		     struct Qdisc **old)
{
	struct cbq_class *cl = (struct cbq_class*)arg;

	if (cl) {
		if (new == NULL) {
			if ((new = qdisc_create_dflt(sch->dev, &pfifo_qdisc_ops)) == NULL)
				return -ENOBUFS;
		} else {
#ifdef CONFIG_NET_CLS_POLICE
			if (cl->police == TC_POLICE_RECLASSIFY)
				new->reshape_fail = cbq_reshape_fail;
#endif
		}
		sch_tree_lock(sch);
		*old = cl->q;
		cl->q = new;
		sch->q.qlen -= (*old)->q.qlen;
		qdisc_reset(*old);
		sch_tree_unlock(sch);

		return 0;
	}
	return -ENOENT;
}