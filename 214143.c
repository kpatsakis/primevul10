static int uncompress_file(struct archive_read* a) {
	int ret;

	while(1) {
		/* Sometimes the uncompression function will return a
		 * 'retry' signal. If this will happen, we have to retry
		 * the function. */
		ret = do_uncompress_file(a);
		if(ret != ARCHIVE_RETRY)
			return ret;
	}
}