static void cbq_ovl_drop(struct cbq_class *cl)
{
	if (cl->q->ops->drop)
		if (cl->q->ops->drop(cl->q))
			cl->qdisc->q.qlen--;
	cl->xstats.overactions++;
	cbq_ovl_classic(cl);
}