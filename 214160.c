static int bid_standard(struct archive_read* a) {
	const uint8_t* p;

	if(!read_ahead(a, rar5_signature_size, &p))
		return -1;

	if(!memcmp(rar5_signature, p, rar5_signature_size))
		return 30;

	return -1;
}