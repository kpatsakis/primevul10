static inline int sadb_key_len(const struct sadb_key *key)
{
	int key_bytes = DIV_ROUND_UP(key->sadb_key_bits, 8);

	return DIV_ROUND_UP(sizeof(struct sadb_key) + key_bytes,
			    sizeof(uint64_t));
}