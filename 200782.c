void sctp_assoc_control_transport(struct sctp_association *asoc,
				  struct sctp_transport *transport,
				  sctp_transport_cmd_t command,
				  sctp_sn_error_t error)
{
	struct sctp_transport *t = NULL;
	struct sctp_transport *first;
	struct sctp_transport *second;
	struct sctp_ulpevent *event;
	struct sockaddr_storage addr;
	int spc_state = 0;

	/* Record the transition on the transport.  */
	switch (command) {
	case SCTP_TRANSPORT_UP:
		/* If we are moving from UNCONFIRMED state due
		 * to heartbeat success, report the SCTP_ADDR_CONFIRMED
		 * state to the user, otherwise report SCTP_ADDR_AVAILABLE.
		 */
		if (SCTP_UNCONFIRMED == transport->state &&
		    SCTP_HEARTBEAT_SUCCESS == error)
			spc_state = SCTP_ADDR_CONFIRMED;
		else
			spc_state = SCTP_ADDR_AVAILABLE;
		transport->state = SCTP_ACTIVE;
		break;

	case SCTP_TRANSPORT_DOWN:
		/* if the transort was never confirmed, do not transition it
		 * to inactive state.
		 */
		if (transport->state != SCTP_UNCONFIRMED)
			transport->state = SCTP_INACTIVE;

		spc_state = SCTP_ADDR_UNREACHABLE;
		break;

	default:
		return;
	}

	/* Generate and send a SCTP_PEER_ADDR_CHANGE notification to the
	 * user.
	 */
	memset(&addr, 0, sizeof(struct sockaddr_storage));
	memcpy(&addr, &transport->ipaddr, transport->af_specific->sockaddr_len);
	event = sctp_ulpevent_make_peer_addr_change(asoc, &addr,
				0, spc_state, error, GFP_ATOMIC);
	if (event)
		sctp_ulpq_tail_event(&asoc->ulpq, event);

	/* Select new active and retran paths. */

	/* Look for the two most recently used active transports.
	 *
	 * This code produces the wrong ordering whenever jiffies
	 * rolls over, but we still get usable transports, so we don't
	 * worry about it.
	 */
	first = NULL; second = NULL;

	list_for_each_entry(t, &asoc->peer.transport_addr_list,
			transports) {

		if ((t->state == SCTP_INACTIVE) ||
		    (t->state == SCTP_UNCONFIRMED))
			continue;
		if (!first || t->last_time_heard > first->last_time_heard) {
			second = first;
			first = t;
		}
		if (!second || t->last_time_heard > second->last_time_heard)
			second = t;
	}

	/* RFC 2960 6.4 Multi-Homed SCTP Endpoints
	 *
	 * By default, an endpoint should always transmit to the
	 * primary path, unless the SCTP user explicitly specifies the
	 * destination transport address (and possibly source
	 * transport address) to use.
	 *
	 * [If the primary is active but not most recent, bump the most
	 * recently used transport.]
	 */
	if (((asoc->peer.primary_path->state == SCTP_ACTIVE) ||
	     (asoc->peer.primary_path->state == SCTP_UNKNOWN)) &&
	    first != asoc->peer.primary_path) {
		second = first;
		first = asoc->peer.primary_path;
	}

	/* If we failed to find a usable transport, just camp on the
	 * primary, even if it is inactive.
	 */
	if (!first) {
		first = asoc->peer.primary_path;
		second = asoc->peer.primary_path;
	}

	/* Set the active and retran transports.  */
	asoc->peer.active_path = first;
	asoc->peer.retran_path = second;
}