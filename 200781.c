void sctp_assoc_update_retran_path(struct sctp_association *asoc)
{
	struct sctp_transport *t, *next;
	struct list_head *head = &asoc->peer.transport_addr_list;
	struct list_head *pos;

	if (asoc->peer.transport_count == 1)
		return;

	/* Find the next transport in a round-robin fashion. */
	t = asoc->peer.retran_path;
	pos = &t->transports;
	next = NULL;

	while (1) {
		/* Skip the head. */
		if (pos->next == head)
			pos = head->next;
		else
			pos = pos->next;

		t = list_entry(pos, struct sctp_transport, transports);

		/* We have exhausted the list, but didn't find any
		 * other active transports.  If so, use the next
		 * transport.
		 */
		if (t == asoc->peer.retran_path) {
			t = next;
			break;
		}

		/* Try to find an active transport. */

		if ((t->state == SCTP_ACTIVE) ||
		    (t->state == SCTP_UNKNOWN)) {
			break;
		} else {
			/* Keep track of the next transport in case
			 * we don't find any active transport.
			 */
			if (!next)
				next = t;
		}
	}

	asoc->peer.retran_path = t;

	SCTP_DEBUG_PRINTK_IPADDR("sctp_assoc_update_retran_path:association"
				 " %p addr: ",
				 " port: %d\n",
				 asoc,
				 (&t->ipaddr),
				 ntohs(t->ipaddr.v4.sin_port));
}