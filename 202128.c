static void cbq_undelay(unsigned long arg)
{
	struct Qdisc *sch = (struct Qdisc*)arg;
	struct cbq_sched_data *q = qdisc_priv(sch);
	long delay = 0;
	unsigned pmask;

	pmask = q->pmask;
	q->pmask = 0;

	while (pmask) {
		int prio = ffz(~pmask);
		long tmp;

		pmask &= ~(1<<prio);

		tmp = cbq_undelay_prio(q, prio);
		if (tmp > 0) {
			q->pmask |= 1<<prio;
			if (tmp < delay || delay == 0)
				delay = tmp;
		}
	}

	if (delay) {
		q->delay_timer.expires = jiffies + delay;
		add_timer(&q->delay_timer);
	}

	sch->flags &= ~TCQ_F_THROTTLED;
	netif_schedule(sch->dev);
}