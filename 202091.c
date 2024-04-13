static unsigned long cbq_get(struct Qdisc *sch, u32 classid)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *cl = cbq_class_lookup(q, classid);

	if (cl) {
		cl->refcnt++;
		return (unsigned long)cl;
	}
	return 0;
}