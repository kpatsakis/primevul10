cbq_change_class(struct Qdisc *sch, u32 classid, u32 parentid, struct rtattr **tca,
		 unsigned long *arg)
{
	int err;
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *cl = (struct cbq_class*)*arg;
	struct rtattr *opt = tca[TCA_OPTIONS-1];
	struct rtattr *tb[TCA_CBQ_MAX];
	struct cbq_class *parent;
	struct qdisc_rate_table *rtab = NULL;

	if (opt==NULL || rtattr_parse_nested(tb, TCA_CBQ_MAX, opt))
		return -EINVAL;

	if (tb[TCA_CBQ_OVL_STRATEGY-1] &&
	    RTA_PAYLOAD(tb[TCA_CBQ_OVL_STRATEGY-1]) < sizeof(struct tc_cbq_ovl))
		return -EINVAL;

	if (tb[TCA_CBQ_FOPT-1] &&
	    RTA_PAYLOAD(tb[TCA_CBQ_FOPT-1]) < sizeof(struct tc_cbq_fopt))
		return -EINVAL;

	if (tb[TCA_CBQ_RATE-1] &&
	    RTA_PAYLOAD(tb[TCA_CBQ_RATE-1]) < sizeof(struct tc_ratespec))
			return -EINVAL;

	if (tb[TCA_CBQ_LSSOPT-1] &&
	    RTA_PAYLOAD(tb[TCA_CBQ_LSSOPT-1]) < sizeof(struct tc_cbq_lssopt))
			return -EINVAL;

	if (tb[TCA_CBQ_WRROPT-1] &&
	    RTA_PAYLOAD(tb[TCA_CBQ_WRROPT-1]) < sizeof(struct tc_cbq_wrropt))
			return -EINVAL;

#ifdef CONFIG_NET_CLS_POLICE
	if (tb[TCA_CBQ_POLICE-1] &&
	    RTA_PAYLOAD(tb[TCA_CBQ_POLICE-1]) < sizeof(struct tc_cbq_police))
			return -EINVAL;
#endif

	if (cl) {
		/* Check parent */
		if (parentid) {
			if (cl->tparent && cl->tparent->classid != parentid)
				return -EINVAL;
			if (!cl->tparent && parentid != TC_H_ROOT)
				return -EINVAL;
		}

		if (tb[TCA_CBQ_RATE-1]) {
			rtab = qdisc_get_rtab(RTA_DATA(tb[TCA_CBQ_RATE-1]), tb[TCA_CBQ_RTAB-1]);
			if (rtab == NULL)
				return -EINVAL;
		}

		/* Change class parameters */
		sch_tree_lock(sch);

		if (cl->next_alive != NULL)
			cbq_deactivate_class(cl);

		if (rtab) {
			rtab = xchg(&cl->R_tab, rtab);
			qdisc_put_rtab(rtab);
		}

		if (tb[TCA_CBQ_LSSOPT-1])
			cbq_set_lss(cl, RTA_DATA(tb[TCA_CBQ_LSSOPT-1]));

		if (tb[TCA_CBQ_WRROPT-1]) {
			cbq_rmprio(q, cl);
			cbq_set_wrr(cl, RTA_DATA(tb[TCA_CBQ_WRROPT-1]));
		}

		if (tb[TCA_CBQ_OVL_STRATEGY-1])
			cbq_set_overlimit(cl, RTA_DATA(tb[TCA_CBQ_OVL_STRATEGY-1]));

#ifdef CONFIG_NET_CLS_POLICE
		if (tb[TCA_CBQ_POLICE-1])
			cbq_set_police(cl, RTA_DATA(tb[TCA_CBQ_POLICE-1]));
#endif

		if (tb[TCA_CBQ_FOPT-1])
			cbq_set_fopt(cl, RTA_DATA(tb[TCA_CBQ_FOPT-1]));

		if (cl->q->q.qlen)
			cbq_activate_class(cl);

		sch_tree_unlock(sch);

#ifdef CONFIG_NET_ESTIMATOR
		if (tca[TCA_RATE-1])
			gen_replace_estimator(&cl->bstats, &cl->rate_est,
				cl->stats_lock, tca[TCA_RATE-1]);
#endif
		return 0;
	}

	if (parentid == TC_H_ROOT)
		return -EINVAL;

	if (tb[TCA_CBQ_WRROPT-1] == NULL || tb[TCA_CBQ_RATE-1] == NULL ||
	    tb[TCA_CBQ_LSSOPT-1] == NULL)
		return -EINVAL;

	rtab = qdisc_get_rtab(RTA_DATA(tb[TCA_CBQ_RATE-1]), tb[TCA_CBQ_RTAB-1]);
	if (rtab == NULL)
		return -EINVAL;

	if (classid) {
		err = -EINVAL;
		if (TC_H_MAJ(classid^sch->handle) || cbq_class_lookup(q, classid))
			goto failure;
	} else {
		int i;
		classid = TC_H_MAKE(sch->handle,0x8000);

		for (i=0; i<0x8000; i++) {
			if (++q->hgenerator >= 0x8000)
				q->hgenerator = 1;
			if (cbq_class_lookup(q, classid|q->hgenerator) == NULL)
				break;
		}
		err = -ENOSR;
		if (i >= 0x8000)
			goto failure;
		classid = classid|q->hgenerator;
	}

	parent = &q->link;
	if (parentid) {
		parent = cbq_class_lookup(q, parentid);
		err = -EINVAL;
		if (parent == NULL)
			goto failure;
	}

	err = -ENOBUFS;
	cl = kmalloc(sizeof(*cl), GFP_KERNEL);
	if (cl == NULL)
		goto failure;
	memset(cl, 0, sizeof(*cl));
	cl->R_tab = rtab;
	rtab = NULL;
	cl->refcnt = 1;
	if (!(cl->q = qdisc_create_dflt(sch->dev, &pfifo_qdisc_ops)))
		cl->q = &noop_qdisc;
	cl->classid = classid;
	cl->tparent = parent;
	cl->qdisc = sch;
	cl->allot = parent->allot;
	cl->quantum = cl->allot;
	cl->weight = cl->R_tab->rate.rate;
	cl->stats_lock = &sch->dev->queue_lock;

	sch_tree_lock(sch);
	cbq_link_class(cl);
	cl->borrow = cl->tparent;
	if (cl->tparent != &q->link)
		cl->share = cl->tparent;
	cbq_adjust_levels(parent);
	cl->minidle = -0x7FFFFFFF;
	cbq_set_lss(cl, RTA_DATA(tb[TCA_CBQ_LSSOPT-1]));
	cbq_set_wrr(cl, RTA_DATA(tb[TCA_CBQ_WRROPT-1]));
	if (cl->ewma_log==0)
		cl->ewma_log = q->link.ewma_log;
	if (cl->maxidle==0)
		cl->maxidle = q->link.maxidle;
	if (cl->avpkt==0)
		cl->avpkt = q->link.avpkt;
	cl->overlimit = cbq_ovl_classic;
	if (tb[TCA_CBQ_OVL_STRATEGY-1])
		cbq_set_overlimit(cl, RTA_DATA(tb[TCA_CBQ_OVL_STRATEGY-1]));
#ifdef CONFIG_NET_CLS_POLICE
	if (tb[TCA_CBQ_POLICE-1])
		cbq_set_police(cl, RTA_DATA(tb[TCA_CBQ_POLICE-1]));
#endif
	if (tb[TCA_CBQ_FOPT-1])
		cbq_set_fopt(cl, RTA_DATA(tb[TCA_CBQ_FOPT-1]));
	sch_tree_unlock(sch);

#ifdef CONFIG_NET_ESTIMATOR
	if (tca[TCA_RATE-1])
		gen_new_estimator(&cl->bstats, &cl->rate_est,
			cl->stats_lock, tca[TCA_RATE-1]);
#endif

	*arg = (unsigned long)cl;
	return 0;

failure:
	qdisc_put_rtab(rtab);
	return err;
}