cbq_dequeue(struct Qdisc *sch)
{
	struct sk_buff *skb;
	struct cbq_sched_data *q = qdisc_priv(sch);
	psched_time_t now;
	psched_tdiff_t incr;

	PSCHED_GET_TIME(now);
	incr = PSCHED_TDIFF(now, q->now_rt);

	if (q->tx_class) {
		psched_tdiff_t incr2;
		/* Time integrator. We calculate EOS time
		   by adding expected packet transmission time.
		   If real time is greater, we warp artificial clock,
		   so that:

		   cbq_time = max(real_time, work);
		 */
		incr2 = L2T(&q->link, q->tx_len);
		PSCHED_TADD(q->now, incr2);
		cbq_update(q);
		if ((incr -= incr2) < 0)
			incr = 0;
	}
	PSCHED_TADD(q->now, incr);
	q->now_rt = now;

	for (;;) {
		q->wd_expires = 0;

		skb = cbq_dequeue_1(sch);
		if (skb) {
			sch->q.qlen--;
			sch->flags &= ~TCQ_F_THROTTLED;
			return skb;
		}

		/* All the classes are overlimit.

		   It is possible, if:

		   1. Scheduler is empty.
		   2. Toplevel cutoff inhibited borrowing.
		   3. Root class is overlimit.

		   Reset 2d and 3d conditions and retry.

		   Note, that NS and cbq-2.0 are buggy, peeking
		   an arbitrary class is appropriate for ancestor-only
		   sharing, but not for toplevel algorithm.

		   Our version is better, but slower, because it requires
		   two passes, but it is unavoidable with top-level sharing.
		*/

		if (q->toplevel == TC_CBQ_MAXLEVEL &&
		    PSCHED_IS_PASTPERFECT(q->link.undertime))
			break;

		q->toplevel = TC_CBQ_MAXLEVEL;
		PSCHED_SET_PASTPERFECT(q->link.undertime);
	}

	/* No packets in scheduler or nobody wants to give them to us :-(
	   Sigh... start watchdog timer in the last case. */

	if (sch->q.qlen) {
		sch->qstats.overlimits++;
		if (q->wd_expires) {
			long delay = PSCHED_US2JIFFIE(q->wd_expires);
			if (delay <= 0)
				delay = 1;
			mod_timer(&q->wd_timer, jiffies + delay);
			sch->flags |= TCQ_F_THROTTLED;
		}
	}
	return NULL;
}