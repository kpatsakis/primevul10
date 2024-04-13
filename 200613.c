int sctp_auth_asoc_copy_shkeys(const struct sctp_endpoint *ep,
				struct sctp_association *asoc,
				gfp_t gfp)
{
	struct sctp_shared_key *sh_key;
	struct sctp_shared_key *new;

	BUG_ON(!list_empty(&asoc->endpoint_shared_keys));

	key_for_each(sh_key, &ep->endpoint_shared_keys) {
		new = sctp_auth_shkey_create(sh_key->key_id, gfp);
		if (!new)
			goto nomem;

		new->key = sh_key->key;
		sctp_auth_key_hold(new->key);
		list_add(&new->key_list, &asoc->endpoint_shared_keys);
	}

	return 0;

nomem:
	sctp_auth_destroy_keys(&asoc->endpoint_shared_keys);
	return -ENOMEM;
}