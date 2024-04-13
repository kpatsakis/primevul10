cbq_dump_class_stats(struct Qdisc *sch, unsigned long arg,
	struct gnet_dump *d)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *cl = (struct cbq_class*)arg;

	cl->qstats.qlen = cl->q->q.qlen;
	cl->xstats.avgidle = cl->avgidle;
	cl->xstats.undertime = 0;

	if (!PSCHED_IS_PASTPERFECT(cl->undertime))
		cl->xstats.undertime = PSCHED_TDIFF(cl->undertime, q->now);

	if (gnet_stats_copy_basic(d, &cl->bstats) < 0 ||
#ifdef CONFIG_NET_ESTIMATOR
	    gnet_stats_copy_rate_est(d, &cl->rate_est) < 0 ||
#endif
	    gnet_stats_copy_queue(d, &cl->qstats) < 0)
		return -1;

	return gnet_stats_copy_app(d, &cl->xstats, sizeof(cl->xstats));
}