static struct sctp_association *sctp_association_init(struct sctp_association *asoc,
					  const struct sctp_endpoint *ep,
					  const struct sock *sk,
					  sctp_scope_t scope,
					  gfp_t gfp)
{
	struct sctp_sock *sp;
	int i;
	sctp_paramhdr_t *p;
	int err;

	/* Retrieve the SCTP per socket area.  */
	sp = sctp_sk((struct sock *)sk);

	/* Init all variables to a known value.  */
	memset(asoc, 0, sizeof(struct sctp_association));

	/* Discarding const is appropriate here.  */
	asoc->ep = (struct sctp_endpoint *)ep;
	sctp_endpoint_hold(asoc->ep);

	/* Hold the sock.  */
	asoc->base.sk = (struct sock *)sk;
	sock_hold(asoc->base.sk);

	/* Initialize the common base substructure.  */
	asoc->base.type = SCTP_EP_TYPE_ASSOCIATION;

	/* Initialize the object handling fields.  */
	atomic_set(&asoc->base.refcnt, 1);
	asoc->base.dead = 0;
	asoc->base.malloced = 0;

	/* Initialize the bind addr area.  */
	sctp_bind_addr_init(&asoc->base.bind_addr, ep->base.bind_addr.port);

	asoc->state = SCTP_STATE_CLOSED;

	/* Set these values from the socket values, a conversion between
	 * millsecons to seconds/microseconds must also be done.
	 */
	asoc->cookie_life.tv_sec = sp->assocparams.sasoc_cookie_life / 1000;
	asoc->cookie_life.tv_usec = (sp->assocparams.sasoc_cookie_life % 1000)
					* 1000;
	asoc->frag_point = 0;

	/* Set the association max_retrans and RTO values from the
	 * socket values.
	 */
	asoc->max_retrans = sp->assocparams.sasoc_asocmaxrxt;
	asoc->rto_initial = msecs_to_jiffies(sp->rtoinfo.srto_initial);
	asoc->rto_max = msecs_to_jiffies(sp->rtoinfo.srto_max);
	asoc->rto_min = msecs_to_jiffies(sp->rtoinfo.srto_min);

	asoc->overall_error_count = 0;

	/* Initialize the association's heartbeat interval based on the
	 * sock configured value.
	 */
	asoc->hbinterval = msecs_to_jiffies(sp->hbinterval);

	/* Initialize path max retrans value. */
	asoc->pathmaxrxt = sp->pathmaxrxt;

	/* Initialize default path MTU. */
	asoc->pathmtu = sp->pathmtu;

	/* Set association default SACK delay */
	asoc->sackdelay = msecs_to_jiffies(sp->sackdelay);
	asoc->sackfreq = sp->sackfreq;

	/* Set the association default flags controlling
	 * Heartbeat, SACK delay, and Path MTU Discovery.
	 */
	asoc->param_flags = sp->param_flags;

	/* Initialize the maximum mumber of new data packets that can be sent
	 * in a burst.
	 */
	asoc->max_burst = sp->max_burst;

	/* initialize association timers */
	asoc->timeouts[SCTP_EVENT_TIMEOUT_NONE] = 0;
	asoc->timeouts[SCTP_EVENT_TIMEOUT_T1_COOKIE] = asoc->rto_initial;
	asoc->timeouts[SCTP_EVENT_TIMEOUT_T1_INIT] = asoc->rto_initial;
	asoc->timeouts[SCTP_EVENT_TIMEOUT_T2_SHUTDOWN] = asoc->rto_initial;
	asoc->timeouts[SCTP_EVENT_TIMEOUT_T3_RTX] = 0;
	asoc->timeouts[SCTP_EVENT_TIMEOUT_T4_RTO] = 0;

	/* sctpimpguide Section 2.12.2
	 * If the 'T5-shutdown-guard' timer is used, it SHOULD be set to the
	 * recommended value of 5 times 'RTO.Max'.
	 */
	asoc->timeouts[SCTP_EVENT_TIMEOUT_T5_SHUTDOWN_GUARD]
		= 5 * asoc->rto_max;

	asoc->timeouts[SCTP_EVENT_TIMEOUT_HEARTBEAT] = 0;
	asoc->timeouts[SCTP_EVENT_TIMEOUT_SACK] = asoc->sackdelay;
	asoc->timeouts[SCTP_EVENT_TIMEOUT_AUTOCLOSE] =
		sp->autoclose * HZ;

	/* Initilizes the timers */
	for (i = SCTP_EVENT_TIMEOUT_NONE; i < SCTP_NUM_TIMEOUT_TYPES; ++i)
		setup_timer(&asoc->timers[i], sctp_timer_events[i],
				(unsigned long)asoc);

	/* Pull default initialization values from the sock options.
	 * Note: This assumes that the values have already been
	 * validated in the sock.
	 */
	asoc->c.sinit_max_instreams = sp->initmsg.sinit_max_instreams;
	asoc->c.sinit_num_ostreams  = sp->initmsg.sinit_num_ostreams;
	asoc->max_init_attempts	= sp->initmsg.sinit_max_attempts;

	asoc->max_init_timeo =
		 msecs_to_jiffies(sp->initmsg.sinit_max_init_timeo);

	/* Allocate storage for the ssnmap after the inbound and outbound
	 * streams have been negotiated during Init.
	 */
	asoc->ssnmap = NULL;

	/* Set the local window size for receive.
	 * This is also the rcvbuf space per association.
	 * RFC 6 - A SCTP receiver MUST be able to receive a minimum of
	 * 1500 bytes in one SCTP packet.
	 */
	if ((sk->sk_rcvbuf/2) < SCTP_DEFAULT_MINWINDOW)
		asoc->rwnd = SCTP_DEFAULT_MINWINDOW;
	else
		asoc->rwnd = sk->sk_rcvbuf/2;

	asoc->a_rwnd = asoc->rwnd;

	asoc->rwnd_over = 0;

	/* Use my own max window until I learn something better.  */
	asoc->peer.rwnd = SCTP_DEFAULT_MAXWINDOW;

	/* Set the sndbuf size for transmit.  */
	asoc->sndbuf_used = 0;

	/* Initialize the receive memory counter */
	atomic_set(&asoc->rmem_alloc, 0);

	init_waitqueue_head(&asoc->wait);

	asoc->c.my_vtag = sctp_generate_tag(ep);
	asoc->peer.i.init_tag = 0;     /* INIT needs a vtag of 0. */
	asoc->c.peer_vtag = 0;
	asoc->c.my_ttag   = 0;
	asoc->c.peer_ttag = 0;
	asoc->c.my_port = ep->base.bind_addr.port;

	asoc->c.initial_tsn = sctp_generate_tsn(ep);

	asoc->next_tsn = asoc->c.initial_tsn;

	asoc->ctsn_ack_point = asoc->next_tsn - 1;
	asoc->adv_peer_ack_point = asoc->ctsn_ack_point;
	asoc->highest_sacked = asoc->ctsn_ack_point;
	asoc->last_cwr_tsn = asoc->ctsn_ack_point;
	asoc->unack_data = 0;

	/* ADDIP Section 4.1 Asconf Chunk Procedures
	 *
	 * When an endpoint has an ASCONF signaled change to be sent to the
	 * remote endpoint it should do the following:
	 * ...
	 * A2) a serial number should be assigned to the chunk. The serial
	 * number SHOULD be a monotonically increasing number. The serial
	 * numbers SHOULD be initialized at the start of the
	 * association to the same value as the initial TSN.
	 */
	asoc->addip_serial = asoc->c.initial_tsn;

	INIT_LIST_HEAD(&asoc->addip_chunk_list);
	INIT_LIST_HEAD(&asoc->asconf_ack_list);

	/* Make an empty list of remote transport addresses.  */
	INIT_LIST_HEAD(&asoc->peer.transport_addr_list);
	asoc->peer.transport_count = 0;

	/* RFC 2960 5.1 Normal Establishment of an Association
	 *
	 * After the reception of the first data chunk in an
	 * association the endpoint must immediately respond with a
	 * sack to acknowledge the data chunk.  Subsequent
	 * acknowledgements should be done as described in Section
	 * 6.2.
	 *
	 * [We implement this by telling a new association that it
	 * already received one packet.]
	 */
	asoc->peer.sack_needed = 1;
	asoc->peer.sack_cnt = 0;

	/* Assume that the peer will tell us if he recognizes ASCONF
	 * as part of INIT exchange.
	 * The sctp_addip_noauth option is there for backward compatibilty
	 * and will revert old behavior.
	 */
	asoc->peer.asconf_capable = 0;
	if (sctp_addip_noauth)
		asoc->peer.asconf_capable = 1;

	/* Create an input queue.  */
	sctp_inq_init(&asoc->base.inqueue);
	sctp_inq_set_th_handler(&asoc->base.inqueue, sctp_assoc_bh_rcv);

	/* Create an output queue.  */
	sctp_outq_init(asoc, &asoc->outqueue);

	if (!sctp_ulpq_init(&asoc->ulpq, asoc))
		goto fail_init;

	/* Set up the tsn tracking. */
	sctp_tsnmap_init(&asoc->peer.tsn_map, SCTP_TSN_MAP_SIZE, 0);

	asoc->need_ecne = 0;

	asoc->assoc_id = 0;

	/* Assume that peer would support both address types unless we are
	 * told otherwise.
	 */
	asoc->peer.ipv4_address = 1;
	asoc->peer.ipv6_address = 1;
	INIT_LIST_HEAD(&asoc->asocs);

	asoc->autoclose = sp->autoclose;

	asoc->default_stream = sp->default_stream;
	asoc->default_ppid = sp->default_ppid;
	asoc->default_flags = sp->default_flags;
	asoc->default_context = sp->default_context;
	asoc->default_timetolive = sp->default_timetolive;
	asoc->default_rcv_context = sp->default_rcv_context;

	/* AUTH related initializations */
	INIT_LIST_HEAD(&asoc->endpoint_shared_keys);
	err = sctp_auth_asoc_copy_shkeys(ep, asoc, gfp);
	if (err)
		goto fail_init;

	asoc->active_key_id = ep->active_key_id;
	asoc->asoc_shared_key = NULL;

	asoc->default_hmac_id = 0;
	/* Save the hmacs and chunks list into this association */
	if (ep->auth_hmacs_list)
		memcpy(asoc->c.auth_hmacs, ep->auth_hmacs_list,
			ntohs(ep->auth_hmacs_list->param_hdr.length));
	if (ep->auth_chunk_list)
		memcpy(asoc->c.auth_chunks, ep->auth_chunk_list,
			ntohs(ep->auth_chunk_list->param_hdr.length));

	/* Get the AUTH random number for this association */
	p = (sctp_paramhdr_t *)asoc->c.auth_random;
	p->type = SCTP_PARAM_RANDOM;
	p->length = htons(sizeof(sctp_paramhdr_t) + SCTP_AUTH_RANDOM_LENGTH);
	get_random_bytes(p+1, SCTP_AUTH_RANDOM_LENGTH);

	return asoc;

fail_init:
	sctp_endpoint_put(asoc->ep);
	sock_put(asoc->base.sk);
	return NULL;
}