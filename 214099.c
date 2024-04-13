static int is_valid_filter_block_start(struct rar5* rar,
    uint32_t start)
{
	const int64_t block_start = (ssize_t) start + rar->cstate.write_ptr;
	const int64_t last_bs = rar->cstate.last_block_start;
	const ssize_t last_bl = rar->cstate.last_block_length;

	if(last_bs == 0 || last_bl == 0) {
		/* We didn't have any filters yet, so accept this offset. */
		return 1;
	}

	if(block_start >= last_bs + last_bl) {
		/* Current offset is bigger than last block's end offset, so
		 * accept current offset. */
		return 1;
	}

	/* Any other case is not a normal situation and we should fail. */
	return 0;
}