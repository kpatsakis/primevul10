static int scan_for_signature(struct archive_read* a) {
	const uint8_t* p;
	const int chunk_size = 512;
	ssize_t i;

	/* If we're here, it means we're on an 'unknown territory' data.
	 * There's no indication what kind of data we're reading here.
	 * It could be some text comment, any kind of binary data,
	 * digital sign, dragons, etc.
	 *
	 * We want to find a valid RARv5 magic header inside this unknown
	 * data. */

	/* Is it possible in libarchive to just skip everything until the
	 * end of the file? If so, it would be a better approach than the
	 * current implementation of this function. */

	while(1) {
		if(!read_ahead(a, chunk_size, &p))
			return ARCHIVE_EOF;

		for(i = 0; i < chunk_size - rar5_signature_size; i++) {
			if(memcmp(&p[i], rar5_signature,
			    rar5_signature_size) == 0) {
				/* Consume the number of bytes we've used to
				 * search for the signature, as well as the
				 * number of bytes used by the signature
				 * itself. After this we should be standing
				 * on a valid base block header. */
				(void) consume(a, i + rar5_signature_size);
				return ARCHIVE_OK;
			}
		}

		consume(a, chunk_size);
	}

	return ARCHIVE_FATAL;
}