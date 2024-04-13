static void cbq_normalize_quanta(struct cbq_sched_data *q, int prio)
{
	struct cbq_class *cl;
	unsigned h;

	if (q->quanta[prio] == 0)
		return;

	for (h=0; h<16; h++) {
		for (cl = q->classes[h]; cl; cl = cl->next) {
			/* BUGGGG... Beware! This expression suffer of
			   arithmetic overflows!
			 */
			if (cl->priority == prio) {
				cl->quantum = (cl->weight*cl->allot*q->nclasses[prio])/
					q->quanta[prio];
			}
			if (cl->quantum <= 0 || cl->quantum>32*cl->qdisc->dev->mtu) {
				printk(KERN_WARNING "CBQ: class %08x has bad quantum==%ld, repaired.\n", cl->classid, cl->quantum);
				cl->quantum = cl->qdisc->dev->mtu/2 + 1;
			}
		}
	}
}