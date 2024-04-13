void *sg_encryption_create_entry(void *udta)
{
	GF_CENCSampleEncryptionGroupEntry *entry, *from_entry;
	GF_SAFEALLOC(entry, GF_CENCSampleEncryptionGroupEntry);
	if (!entry) return NULL;
	from_entry = (GF_CENCSampleEncryptionGroupEntry *)udta;
	memcpy(entry, from_entry, sizeof(GF_CENCSampleEncryptionGroupEntry) );
	entry->key_info = gf_malloc(sizeof(u8) * entry->key_info_size);
	if (!entry->key_info) {
		gf_free(entry);
		return NULL;
	}
	memcpy(entry->key_info, from_entry->key_info, entry->key_info_size);
	return entry;
}