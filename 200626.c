int sctp_auth_set_active_key(struct sctp_endpoint *ep,
			     struct sctp_association *asoc,
			     __u16  key_id)
{
	struct sctp_shared_key *key;
	struct list_head *sh_keys;
	int found = 0;

	/* The key identifier MUST correst to an existing key */
	if (asoc)
		sh_keys = &asoc->endpoint_shared_keys;
	else
		sh_keys = &ep->endpoint_shared_keys;

	key_for_each(key, sh_keys) {
		if (key->key_id == key_id) {
			found = 1;
			break;
		}
	}

	if (!found)
		return -EINVAL;

	if (asoc) {
		asoc->active_key_id = key_id;
		sctp_auth_asoc_init_active_key(asoc, GFP_KERNEL);
	} else
		ep->active_key_id = key_id;

	return 0;
}