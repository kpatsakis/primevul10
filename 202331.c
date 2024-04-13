dev_graft_qdisc(struct net_device *dev, struct Qdisc *qdisc)
{
	struct Qdisc *oqdisc;

	if (dev->flags & IFF_UP)
		dev_deactivate(dev);

	qdisc_lock_tree(dev);
	if (qdisc && qdisc->flags&TCQ_F_INGRESS) {
		oqdisc = dev->qdisc_ingress;
		/* Prune old scheduler */
		if (oqdisc && atomic_read(&oqdisc->refcnt) <= 1) {
			/* delete */
			qdisc_reset(oqdisc);
			dev->qdisc_ingress = NULL;
		} else {  /* new */
			dev->qdisc_ingress = qdisc;
		}

	} else {

		oqdisc = dev->qdisc_sleeping;

		/* Prune old scheduler */
		if (oqdisc && atomic_read(&oqdisc->refcnt) <= 1)
			qdisc_reset(oqdisc);

		/* ... and graft new one */
		if (qdisc == NULL)
			qdisc = &noop_qdisc;
		dev->qdisc_sleeping = qdisc;
		dev->qdisc = &noop_qdisc;
	}

	qdisc_unlock_tree(dev);

	if (dev->flags & IFF_UP)
		dev_activate(dev);

	return oqdisc;
}