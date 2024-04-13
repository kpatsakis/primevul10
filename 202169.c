cbq_update(struct cbq_sched_data *q)
{
	struct cbq_class *this = q->tx_class;
	struct cbq_class *cl = this;
	int len = q->tx_len;

	q->tx_class = NULL;

	for ( ; cl; cl = cl->share) {
		long avgidle = cl->avgidle;
		long idle;

		cl->bstats.packets++;
		cl->bstats.bytes += len;

		/*
		   (now - last) is total time between packet right edges.
		   (last_pktlen/rate) is "virtual" busy time, so that

		         idle = (now - last) - last_pktlen/rate
		 */

		idle = PSCHED_TDIFF(q->now, cl->last);
		if ((unsigned long)idle > 128*1024*1024) {
			avgidle = cl->maxidle;
		} else {
			idle -= L2T(cl, len);

		/* true_avgidle := (1-W)*true_avgidle + W*idle,
		   where W=2^{-ewma_log}. But cl->avgidle is scaled:
		   cl->avgidle == true_avgidle/W,
		   hence:
		 */
			avgidle += idle - (avgidle>>cl->ewma_log);
		}

		if (avgidle <= 0) {
			/* Overlimit or at-limit */

			if (avgidle < cl->minidle)
				avgidle = cl->minidle;

			cl->avgidle = avgidle;

			/* Calculate expected time, when this class
			   will be allowed to send.
			   It will occur, when:
			   (1-W)*true_avgidle + W*delay = 0, i.e.
			   idle = (1/W - 1)*(-true_avgidle)
			   or
			   idle = (1 - W)*(-cl->avgidle);
			 */
			idle = (-avgidle) - ((-avgidle) >> cl->ewma_log);

			/*
			   That is not all.
			   To maintain the rate allocated to the class,
			   we add to undertime virtual clock,
			   necessary to complete transmitted packet.
			   (len/phys_bandwidth has been already passed
			   to the moment of cbq_update)
			 */

			idle -= L2T(&q->link, len);
			idle += L2T(cl, len);

			PSCHED_AUDIT_TDIFF(idle);

			PSCHED_TADD2(q->now, idle, cl->undertime);
		} else {
			/* Underlimit */

			PSCHED_SET_PASTPERFECT(cl->undertime);
			if (avgidle > cl->maxidle)
				cl->avgidle = cl->maxidle;
			else
				cl->avgidle = avgidle;
		}
		cl->last = q->now;
	}

	cbq_update_toplevel(q, this, q->tx_borrowed);
}