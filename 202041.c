cbq_dump_stats(struct Qdisc *sch, struct gnet_dump *d)
{
	struct cbq_sched_data *q = qdisc_priv(sch);

	q->link.xstats.avgidle = q->link.avgidle;
	return gnet_stats_copy_app(d, &q->link.xstats, sizeof(q->link.xstats));
}