int sctp_auth_del_key_id(struct sctp_endpoint *ep,
			 struct sctp_association *asoc,
			 __u16  key_id)
{
	struct sctp_shared_key *key;
	struct list_head *sh_keys;
	int found = 0;

	/* The key identifier MUST NOT be the current active key
	 * The key identifier MUST correst to an existing key
	 */
	if (asoc) {
		if (asoc->active_key_id == key_id)
			return -EINVAL;

		sh_keys = &asoc->endpoint_shared_keys;
	} else {
		if (ep->active_key_id == key_id)
			return -EINVAL;

		sh_keys = &ep->endpoint_shared_keys;
	}

	key_for_each(key, sh_keys) {
		if (key->key_id == key_id) {
			found = 1;
			break;
		}
	}

	if (!found)
		return -EINVAL;

	/* Delete the shared key */
	list_del_init(&key->key_list);
	sctp_auth_shkey_free(key);

	return 0;
}