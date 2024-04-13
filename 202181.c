cbq_leaf(struct Qdisc *sch, unsigned long arg)
{
	struct cbq_class *cl = (struct cbq_class*)arg;

	return cl ? cl->q : NULL;
}