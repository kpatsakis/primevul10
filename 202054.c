cbq_dequeue_prio(struct Qdisc *sch, int prio)
{
	struct cbq_sched_data *q = qdisc_priv(sch);
	struct cbq_class *cl_tail, *cl_prev, *cl;
	struct sk_buff *skb;
	int deficit;

	cl_tail = cl_prev = q->active[prio];
	cl = cl_prev->next_alive;

	do {
		deficit = 0;

		/* Start round */
		do {
			struct cbq_class *borrow = cl;

			if (cl->q->q.qlen &&
			    (borrow = cbq_under_limit(cl)) == NULL)
				goto skip_class;

			if (cl->deficit <= 0) {
				/* Class exhausted its allotment per
				   this round. Switch to the next one.
				 */
				deficit = 1;
				cl->deficit += cl->quantum;
				goto next_class;
			}

			skb = cl->q->dequeue(cl->q);

			/* Class did not give us any skb :-(
			   It could occur even if cl->q->q.qlen != 0 
			   f.e. if cl->q == "tbf"
			 */
			if (skb == NULL)
				goto skip_class;

			cl->deficit -= skb->len;
			q->tx_class = cl;
			q->tx_borrowed = borrow;
			if (borrow != cl) {
#ifndef CBQ_XSTATS_BORROWS_BYTES
				borrow->xstats.borrows++;
				cl->xstats.borrows++;
#else
				borrow->xstats.borrows += skb->len;
				cl->xstats.borrows += skb->len;
#endif
			}
			q->tx_len = skb->len;

			if (cl->deficit <= 0) {
				q->active[prio] = cl;
				cl = cl->next_alive;
				cl->deficit += cl->quantum;
			}
			return skb;

skip_class:
			if (cl->q->q.qlen == 0 || prio != cl->cpriority) {
				/* Class is empty or penalized.
				   Unlink it from active chain.
				 */
				cl_prev->next_alive = cl->next_alive;
				cl->next_alive = NULL;

				/* Did cl_tail point to it? */
				if (cl == cl_tail) {
					/* Repair it! */
					cl_tail = cl_prev;

					/* Was it the last class in this band? */
					if (cl == cl_tail) {
						/* Kill the band! */
						q->active[prio] = NULL;
						q->activemask &= ~(1<<prio);
						if (cl->q->q.qlen)
							cbq_activate_class(cl);
						return NULL;
					}

					q->active[prio] = cl_tail;
				}
				if (cl->q->q.qlen)
					cbq_activate_class(cl);

				cl = cl_prev;
			}

next_class:
			cl_prev = cl;
			cl = cl->next_alive;
		} while (cl_prev != cl_tail);
	} while (deficit);

	q->active[prio] = cl_prev;

	return NULL;
}