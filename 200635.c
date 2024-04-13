void sctp_auth_key_put(struct sctp_auth_bytes *key)
{
	if (!key)
		return;

	if (atomic_dec_and_test(&key->refcnt)) {
		kfree(key);
		SCTP_DBG_OBJCNT_DEC(keys);
	}
}