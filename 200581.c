SCTP_STATIC int sctp_seqpacket_listen(struct sock *sk, int backlog)
{
	struct sctp_sock *sp = sctp_sk(sk);
	struct sctp_endpoint *ep = sp->ep;

	/* Only UDP style sockets that are not peeled off are allowed to
	 * listen().
	 */
	if (!sctp_style(sk, UDP))
		return -EINVAL;

	/* If backlog is zero, disable listening. */
	if (!backlog) {
		if (sctp_sstate(sk, CLOSED))
			return 0;

		sctp_unhash_endpoint(ep);
		sk->sk_state = SCTP_SS_CLOSED;
		return 0;
	}

	/* Return if we are already listening. */
	if (sctp_sstate(sk, LISTENING))
		return 0;

	/*
	 * If a bind() or sctp_bindx() is not called prior to a listen()
	 * call that allows new associations to be accepted, the system
	 * picks an ephemeral port and will choose an address set equivalent
	 * to binding with a wildcard address.
	 *
	 * This is not currently spelled out in the SCTP sockets
	 * extensions draft, but follows the practice as seen in TCP
	 * sockets.
	 *
	 * Additionally, turn off fastreuse flag since we are not listening
	 */
	sk->sk_state = SCTP_SS_LISTENING;
	if (!ep->base.bind_addr.port) {
		if (sctp_autobind(sk))
			return -EAGAIN;
	} else {
		if (sctp_get_port(sk, inet_sk(sk)->num)) {
			sk->sk_state = SCTP_SS_CLOSED;
			return -EADDRINUSE;
		}
		sctp_sk(sk)->bind_hash->fastreuse = 0;
	}

	sctp_hash_endpoint(ep);
	return 0;
}