GF_Err gf_isom_set_sample_cenc_group(GF_ISOFile *movie, u32 track, u32 sample_number, u8 isEncrypted, u8 crypt_byte_block, u8 skip_byte_block, u8 *key_info, u32 key_info_size)
{
	GF_CENCSampleEncryptionGroupEntry entry;
	if (!key_info || (key_info_size<19))
		return GF_BAD_PARAM;

	memset(&entry, 0, sizeof(GF_CENCSampleEncryptionGroupEntry));
	entry.crypt_byte_block = crypt_byte_block;
	entry.skip_byte_block = skip_byte_block;
	entry.IsProtected = isEncrypted;
	entry.key_info = key_info;
	entry.key_info_size = key_info_size;

	return gf_isom_set_sample_group_info(movie, track, 0, sample_number, GF_ISOM_SAMPLE_GROUP_SEIG, 0, &entry, sg_encryption_create_entry, sg_encryption_compare_entry);
}