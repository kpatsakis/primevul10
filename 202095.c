static void cbq_ovl_rclassic(struct cbq_class *cl)
{
	struct cbq_sched_data *q = qdisc_priv(cl->qdisc);
	struct cbq_class *this = cl;

	do {
		if (cl->level > q->toplevel) {
			cl = NULL;
			break;
		}
	} while ((cl = cl->borrow) != NULL);

	if (cl == NULL)
		cl = this;
	cbq_ovl_classic(cl);
}