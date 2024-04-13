static void cbq_unbind_filter(struct Qdisc *sch, unsigned long arg)
{
	struct cbq_class *cl = (struct cbq_class*)arg;

	cl->filters--;
}