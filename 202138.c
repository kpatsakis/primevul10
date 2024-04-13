static void cbq_put(struct Qdisc *sch, unsigned long arg)
{
	struct cbq_class *cl = (struct cbq_class*)arg;

	if (--cl->refcnt == 0) {
#ifdef CONFIG_NET_CLS_POLICE
		struct cbq_sched_data *q = qdisc_priv(sch);

		spin_lock_bh(&sch->dev->queue_lock);
		if (q->rx_class == cl)
			q->rx_class = NULL;
		spin_unlock_bh(&sch->dev->queue_lock);
#endif

		cbq_destroy_class(sch, cl);
	}
}