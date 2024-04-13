void sctp_assoc_rwnd_increase(struct sctp_association *asoc, unsigned len)
{
	struct sctp_chunk *sack;
	struct timer_list *timer;

	if (asoc->rwnd_over) {
		if (asoc->rwnd_over >= len) {
			asoc->rwnd_over -= len;
		} else {
			asoc->rwnd += (len - asoc->rwnd_over);
			asoc->rwnd_over = 0;
		}
	} else {
		asoc->rwnd += len;
	}

	SCTP_DEBUG_PRINTK("%s: asoc %p rwnd increased by %d to (%u, %u) "
			  "- %u\n", __func__, asoc, len, asoc->rwnd,
			  asoc->rwnd_over, asoc->a_rwnd);

	/* Send a window update SACK if the rwnd has increased by at least the
	 * minimum of the association's PMTU and half of the receive buffer.
	 * The algorithm used is similar to the one described in
	 * Section 4.2.3.3 of RFC 1122.
	 */
	if (sctp_peer_needs_update(asoc)) {
		asoc->a_rwnd = asoc->rwnd;
		SCTP_DEBUG_PRINTK("%s: Sending window update SACK- asoc: %p "
				  "rwnd: %u a_rwnd: %u\n", __func__,
				  asoc, asoc->rwnd, asoc->a_rwnd);
		sack = sctp_make_sack(asoc);
		if (!sack)
			return;

		asoc->peer.sack_needed = 0;

		sctp_outq_tail(&asoc->outqueue, sack);

		/* Stop the SACK timer.  */
		timer = &asoc->timers[SCTP_EVENT_TIMEOUT_SACK];
		if (timer_pending(timer) && del_timer(timer))
			sctp_association_put(asoc);
	}
}