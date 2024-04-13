static void cbq_destroy_class(struct Qdisc *sch, struct cbq_class *cl)
{
	struct cbq_sched_data *q = qdisc_priv(sch);

	BUG_TRAP(!cl->filters);

	cbq_destroy_filters(cl);
	qdisc_destroy(cl->q);
	qdisc_put_rtab(cl->R_tab);
#ifdef CONFIG_NET_ESTIMATOR
	gen_kill_estimator(&cl->bstats, &cl->rate_est);
#endif
	if (cl != &q->link)
		kfree(cl);
}