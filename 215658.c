Bool sg_encryption_compare_entry(void *udta, void *_entry)
{
	GF_CENCSampleEncryptionGroupEntry *entry = (GF_CENCSampleEncryptionGroupEntry *)_entry;
	GF_CENCSampleEncryptionGroupEntry *with_entry = (GF_CENCSampleEncryptionGroupEntry *)udta;

	if (entry->IsProtected != with_entry->IsProtected) return GF_FALSE;
	if (entry->skip_byte_block != with_entry->skip_byte_block) return GF_FALSE;
	if (entry->crypt_byte_block != with_entry->crypt_byte_block) return GF_FALSE;
	if (entry->key_info_size != with_entry->key_info_size) return GF_FALSE;

	if (!memcmp(entry->key_info, with_entry->key_info, with_entry->key_info_size))
		return GF_TRUE;
	return GF_FALSE;
}