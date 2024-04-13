
static inline unsigned ext4_rec_len_from_disk(__le16 dlen)
{
	unsigned len = le16_to_cpu(dlen);

	if (len == EXT4_MAX_REC_LEN)
		return 1 << 16;
	return len;