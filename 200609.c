struct sctp_shared_key *sctp_auth_get_shkey(
				const struct sctp_association *asoc,
				__u16 key_id)
{
	struct sctp_shared_key *key;

	/* First search associations set of endpoint pair shared keys */
	key_for_each(key, &asoc->endpoint_shared_keys) {
		if (key->key_id == key_id)
			return key;
	}

	return NULL;
}