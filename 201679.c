write_tag_11_packet(char *dest, size_t *remaining_bytes, char *contents,
		    size_t contents_length, size_t *packet_length)
{
	size_t packet_size_length;
	size_t max_packet_size;
	int rc = 0;

	(*packet_length) = 0;
	/* This format is inspired by OpenPGP; see RFC 2440
	 * packet tag 11 */
	max_packet_size = (1                   /* Tag 11 identifier */
			   + 3                 /* Max Tag 11 packet size */
			   + 1                 /* Binary format specifier */
			   + 1                 /* Filename length */
			   + 8                 /* Filename ("_CONSOLE") */
			   + 4                 /* Modification date */
			   + contents_length); /* Literal data */
	if (max_packet_size > (*remaining_bytes)) {
		printk(KERN_ERR "Packet length larger than maximum allowable; "
		       "need up to [%td] bytes, but there are only [%td] "
		       "available\n", max_packet_size, (*remaining_bytes));
		rc = -EINVAL;
		goto out;
	}
	dest[(*packet_length)++] = ECRYPTFS_TAG_11_PACKET_TYPE;
	rc = ecryptfs_write_packet_length(&dest[(*packet_length)],
					  (max_packet_size - 4),
					  &packet_size_length);
	if (rc) {
		printk(KERN_ERR "Error generating tag 11 packet header; cannot "
		       "generate packet length. rc = [%d]\n", rc);
		goto out;
	}
	(*packet_length) += packet_size_length;
	dest[(*packet_length)++] = 0x62; /* binary data format specifier */
	dest[(*packet_length)++] = 8;
	memcpy(&dest[(*packet_length)], "_CONSOLE", 8);
	(*packet_length) += 8;
	memset(&dest[(*packet_length)], 0x00, 4);
	(*packet_length) += 4;
	memcpy(&dest[(*packet_length)], contents, contents_length);
	(*packet_length) += contents_length;
 out:
	if (rc)
		(*packet_length) = 0;
	else
		(*remaining_bytes) -= (*packet_length);
	return rc;
}