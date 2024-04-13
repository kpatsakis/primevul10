static void cbq_watchdog(unsigned long arg)
{
	struct Qdisc *sch = (struct Qdisc*)arg;

	sch->flags &= ~TCQ_F_THROTTLED;
	netif_schedule(sch->dev);
}