cbq_classify(struct sk_buff *skb, struct Qdisc *sch, int *qerr)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *head = &q->link;
	struct cbq_class **defmap;
	struct cbq_class *cl = NULL;
	u32 prio = skb->priority;
	struct tcf_result res;

	/*
	 *  Step 1. If skb->priority points to one of our classes, use it.
	 */
	if (TC_H_MAJ(prio^sch->handle) == 0 &&
	    (cl = cbq_class_lookup(q, prio)) != NULL)
		return cl;

	*qerr = NET_XMIT_DROP;
	for (;;) {
		int result = 0;
		defmap = head->defaults;

		/*
		 * Step 2+n. Apply classifier.
		 */
		if (!head->filter_list || (result = tc_classify(skb, head->filter_list, &res)) < 0)
			goto fallback;

		if ((cl = (void*)res.class) == NULL) {
			if (TC_H_MAJ(res.classid))
				cl = cbq_class_lookup(q, res.classid);
			else if ((cl = defmap[res.classid&TC_PRIO_MAX]) == NULL)
				cl = defmap[TC_PRIO_BESTEFFORT];

			if (cl == NULL || cl->level >= head->level)
				goto fallback;
		}

#ifdef CONFIG_NET_CLS_ACT
		switch (result) {
		case TC_ACT_QUEUED:
		case TC_ACT_STOLEN: 
			*qerr = NET_XMIT_SUCCESS;
		case TC_ACT_SHOT:
			return NULL;
		}
#elif defined(CONFIG_NET_CLS_POLICE)
		switch (result) {
		case TC_POLICE_RECLASSIFY:
			return cbq_reclassify(skb, cl);
		case TC_POLICE_SHOT:
			return NULL;
		default:
			break;
		}
#endif
		if (cl->level == 0)
			return cl;

		/*
		 * Step 3+n. If classifier selected a link sharing class,
		 *	   apply agency specific classifier.
		 *	   Repeat this procdure until we hit a leaf node.
		 */
		head = cl;
	}

fallback:
	cl = head;

	/*
	 * Step 4. No success...
	 */
	if (TC_H_MAJ(prio) == 0 &&
	    !(cl = head->defaults[prio&TC_PRIO_MAX]) &&
	    !(cl = head->defaults[TC_PRIO_BESTEFFORT]))
		return head;

	return cl;
}