void sctp_auth_calculate_hmac(const struct sctp_association *asoc,
			      struct sk_buff *skb,
			      struct sctp_auth_chunk *auth,
			      gfp_t gfp)
{
	struct scatterlist sg;
	struct hash_desc desc;
	struct sctp_auth_bytes *asoc_key;
	__u16 key_id, hmac_id;
	__u8 *digest;
	unsigned char *end;
	int free_key = 0;

	/* Extract the info we need:
	 * - hmac id
	 * - key id
	 */
	key_id = ntohs(auth->auth_hdr.shkey_id);
	hmac_id = ntohs(auth->auth_hdr.hmac_id);

	if (key_id == asoc->active_key_id)
		asoc_key = asoc->asoc_shared_key;
	else {
		struct sctp_shared_key *ep_key;

		ep_key = sctp_auth_get_shkey(asoc, key_id);
		if (!ep_key)
			return;

		asoc_key = sctp_auth_asoc_create_secret(asoc, ep_key, gfp);
		if (!asoc_key)
			return;

		free_key = 1;
	}

	/* set up scatter list */
	end = skb_tail_pointer(skb);
	sg_init_one(&sg, auth, end - (unsigned char *)auth);

	desc.tfm = asoc->ep->auth_hmacs[hmac_id];
	desc.flags = 0;

	digest = auth->auth_hdr.hmac;
	if (crypto_hash_setkey(desc.tfm, &asoc_key->data[0], asoc_key->len))
		goto free;

	crypto_hash_digest(&desc, &sg, sg.length, digest);

free:
	if (free_key)
		sctp_auth_key_put(asoc_key);
}