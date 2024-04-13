static int rar5_read_data_skip(struct archive_read *a) {
	struct rar5* rar = get_context(a);

	if(rar->main.solid) {
		/* In solid archives, instead of skipping the data, we need to
		 * extract it, and dispose the result. The side effect of this
		 * operation will be setting up the initial window buffer state
		 * needed to be able to extract the selected file. */

		int ret;

		/* Make sure to process all blocks in the compressed stream. */
		while(rar->file.bytes_remaining > 0) {
			/* Setting the "skip mode" will allow us to skip
			 * checksum checks during data skipping. Checking the
			 * checksum of skipped data isn't really necessary and
			 * it's only slowing things down.
			 *
			 * This is incremented instead of setting to 1 because
			 * this data skipping function can be called
			 * recursively. */
			rar->skip_mode++;

			/* We're disposing 1 block of data, so we use triple
			 * NULLs in arguments. */
			ret = rar5_read_data(a, NULL, NULL, NULL);

			/* Turn off "skip mode". */
			rar->skip_mode--;

			if(ret < 0 || ret == ARCHIVE_EOF) {
				/* Propagate any potential error conditions
				 * to the caller. */
				return ret;
			}
		}
	} else {
		/* In standard archives, we can just jump over the compressed
		 * stream. Each file in non-solid archives starts from an empty
		 * window buffer. */

		if(ARCHIVE_OK != consume(a, rar->file.bytes_remaining)) {
			return ARCHIVE_FATAL;
		}

		rar->file.bytes_remaining = 0;
	}

	return ARCHIVE_OK;
}