int sctp_auth_send_cid(sctp_cid_t chunk, const struct sctp_association *asoc)
{
	if (!sctp_auth_enable || !asoc || !asoc->peer.auth_capable)
		return 0;

	return __sctp_auth_cid(chunk, asoc->peer.peer_chunks);
}