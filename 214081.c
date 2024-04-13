static int rar5_init(struct rar5* rar) {
	ssize_t i;

	memset(rar, 0, sizeof(struct rar5));

	/* Decrypt the magic signature pattern. Check the comment near the
	 * `rar5_signature` symbol to read the rationale behind this. */

	if(rar5_signature[0] == 243) {
		for(i = 0; i < rar5_signature_size; i++) {
			rar5_signature[i] ^= 0xA1;
		}
	}

	if(CDE_OK != cdeque_init(&rar->cstate.filters, 8192))
		return ARCHIVE_FATAL;

	return ARCHIVE_OK;
}