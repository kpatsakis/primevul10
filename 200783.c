void sctp_assoc_sync_pmtu(struct sctp_association *asoc)
{
	struct sctp_transport *t;
	__u32 pmtu = 0;

	if (!asoc)
		return;

	/* Get the lowest pmtu of all the transports. */
	list_for_each_entry(t, &asoc->peer.transport_addr_list,
				transports) {
		if (t->pmtu_pending && t->dst) {
			sctp_transport_update_pmtu(t, dst_mtu(t->dst));
			t->pmtu_pending = 0;
		}
		if (!pmtu || (t->pathmtu < pmtu))
			pmtu = t->pathmtu;
	}

	if (pmtu) {
		struct sctp_sock *sp = sctp_sk(asoc->base.sk);
		asoc->pathmtu = pmtu;
		asoc->frag_point = sctp_frag_point(sp, pmtu);
	}

	SCTP_DEBUG_PRINTK("%s: asoc:%p, pmtu:%d, frag_point:%d\n",
			  __func__, asoc, asoc->pathmtu, asoc->frag_point);
}