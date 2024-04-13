static int cbq_init(struct Qdisc *sch, struct rtattr *opt)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct rtattr *tb[TCA_CBQ_MAX];
	struct tc_ratespec *r;

	if (rtattr_parse_nested(tb, TCA_CBQ_MAX, opt) < 0 ||
	    tb[TCA_CBQ_RTAB-1] == NULL || tb[TCA_CBQ_RATE-1] == NULL ||
	    RTA_PAYLOAD(tb[TCA_CBQ_RATE-1]) < sizeof(struct tc_ratespec))
		return -EINVAL;

	if (tb[TCA_CBQ_LSSOPT-1] &&
	    RTA_PAYLOAD(tb[TCA_CBQ_LSSOPT-1]) < sizeof(struct tc_cbq_lssopt))
		return -EINVAL;

	r = RTA_DATA(tb[TCA_CBQ_RATE-1]);

	if ((q->link.R_tab = qdisc_get_rtab(r, tb[TCA_CBQ_RTAB-1])) == NULL)
		return -EINVAL;

	q->link.refcnt = 1;
	q->link.sibling = &q->link;
	q->link.classid = sch->handle;
	q->link.qdisc = sch;
	if (!(q->link.q = qdisc_create_dflt(sch->dev, &pfifo_qdisc_ops)))
		q->link.q = &noop_qdisc;

	q->link.priority = TC_CBQ_MAXPRIO-1;
	q->link.priority2 = TC_CBQ_MAXPRIO-1;
	q->link.cpriority = TC_CBQ_MAXPRIO-1;
	q->link.ovl_strategy = TC_CBQ_OVL_CLASSIC;
	q->link.overlimit = cbq_ovl_classic;
	q->link.allot = psched_mtu(sch->dev);
	q->link.quantum = q->link.allot;
	q->link.weight = q->link.R_tab->rate.rate;

	q->link.ewma_log = TC_CBQ_DEF_EWMA;
	q->link.avpkt = q->link.allot/2;
	q->link.minidle = -0x7FFFFFFF;
	q->link.stats_lock = &sch->dev->queue_lock;

	init_timer(&q->wd_timer);
	q->wd_timer.data = (unsigned long)sch;
	q->wd_timer.function = cbq_watchdog;
	init_timer(&q->delay_timer);
	q->delay_timer.data = (unsigned long)sch;
	q->delay_timer.function = cbq_undelay;
	q->toplevel = TC_CBQ_MAXLEVEL;
	PSCHED_GET_TIME(q->now);
	q->now_rt = q->now;

	cbq_link_class(&q->link);

	if (tb[TCA_CBQ_LSSOPT-1])
		cbq_set_lss(&q->link, RTA_DATA(tb[TCA_CBQ_LSSOPT-1]));

	cbq_addprio(q, &q->link);
	return 0;
}