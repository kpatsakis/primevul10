int sctp_auth_asoc_init_active_key(struct sctp_association *asoc, gfp_t gfp)
{
	struct sctp_auth_bytes	*secret;
	struct sctp_shared_key *ep_key;

	/* If we don't support AUTH, or peer is not capable
	 * we don't need to do anything.
	 */
	if (!sctp_auth_enable || !asoc->peer.auth_capable)
		return 0;

	/* If the key_id is non-zero and we couldn't find an
	 * endpoint pair shared key, we can't compute the
	 * secret.
	 * For key_id 0, endpoint pair shared key is a NULL key.
	 */
	ep_key = sctp_auth_get_shkey(asoc, asoc->active_key_id);
	BUG_ON(!ep_key);

	secret = sctp_auth_asoc_create_secret(asoc, ep_key, gfp);
	if (!secret)
		return -ENOMEM;

	sctp_auth_key_put(asoc->asoc_shared_key);
	asoc->asoc_shared_key = secret;

	return 0;
}