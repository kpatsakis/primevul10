static unsigned long cbq_bind_filter(struct Qdisc *sch, unsigned long parent,
				     u32 classid)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *p = (struct cbq_class*)parent;
	struct cbq_class *cl = cbq_class_lookup(q, classid);

	if (cl) {
		if (p && p->level <= cl->level)
			return 0;
		cl->filters++;
		return (unsigned long)cl;
	}
	return 0;
}