static int verify_key_len(const void *p)
{
	const struct sadb_key *key = p;

	if (sadb_key_len(key) > key->sadb_key_len)
		return -EINVAL;

	return 0;
}