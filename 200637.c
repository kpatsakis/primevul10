void sctp_auth_destroy_keys(struct list_head *keys)
{
	struct sctp_shared_key *ep_key;
	struct sctp_shared_key *tmp;

	if (list_empty(keys))
		return;

	key_for_each_safe(ep_key, tmp, keys) {
		list_del_init(&ep_key->key_list);
		sctp_auth_shkey_free(ep_key);
	}
}